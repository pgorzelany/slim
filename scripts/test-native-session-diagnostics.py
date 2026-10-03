"""Pure public-frame failure tests; synthetic replies are not native acceptance."""
import hashlib
import importlib.util
from pathlib import Path
import struct
import unittest
from unittest import mock


spec = importlib.util.spec_from_file_location('measurement',Path(__file__).with_name('measure-native-session.py'))
measurement = importlib.util.module_from_spec(spec)
spec.loader.exec_module(measurement)


def response(status=3, context=b'captured-context', reason=b'native-build-failed',
             diagnostics=b'', artifact=b''):
    # Independent literal public N layout: version, five signed words, three
    # starts, three hit bytes, four timing words, then four length-bound fields.
    return (b'\x01'+struct.pack('>5q',7,11,status,0,93)+
            struct.pack('>3Q',1,0,0)+b'\x00\x01\x00'+
            struct.pack('>4Q',0,81,0,0)+
            struct.pack('>4I',len(context),len(reason),len(diagnostics),len(artifact))+
            context+reason+diagnostics+artifact)


class NativeSessionDiagnosticsTests(unittest.TestCase):
    def failure(self,payload,tag=b'N'):
        with self.assertRaises(measurement.NativeResponseFailure) as caught:
            measurement.decode(tag,payload)
        return caught.exception.args[0]

    def test_silent_status3_retains_known_header_and_complete_tail(self):
        payload = response()
        facts = self.failure(payload)
        self.assertEqual(facts['problem'],'native-status-rejected')
        self.assertEqual((facts['revision'],facts['status'],facts['profile'],facts['elapsed_ns']),
                         ((7,11),3,0,93))
        self.assertEqual(facts['starts'],(1,0,0))
        self.assertEqual(facts['hits'],(0,1,0))
        self.assertEqual(facts['times'],(0,81,0,0))
        self.assertEqual((facts['context'],facts['reason'],facts['diagnostics'],facts['artifact_bytes']),
                         (b'captured-context',b'native-build-failed',b'',0))
        frame = b'N'+struct.pack('>I',len(payload))+payload
        self.assertEqual(facts['frame_bytes'],len(frame))
        self.assertEqual(facts['payload_bytes'],len(payload))
        self.assertEqual(facts['frame_sha256'],hashlib.sha256(frame).hexdigest())

    def test_compiler_output_past_old_prefix_is_retained_as_bytes(self):
        diagnostics = b'clang: error: '+b'x'*5000+b'\xff\nlast diagnostic\n'
        facts = self.failure(response(diagnostics=diagnostics))
        self.assertEqual(facts['diagnostics'],diagnostics)
        self.assertEqual(facts['reason'],b'native-build-failed')
        self.assertEqual(facts['lengths'][2],len(diagnostics))

    def test_existing_diagnostic_byte_bound_is_complete(self):
        diagnostics = b'x'*1048576
        self.assertEqual(self.failure(response(diagnostics=diagnostics))['diagnostics'],diagnostics)
        facts = self.failure(response(diagnostics=diagnostics+b'x'))
        self.assertEqual(facts['problem'],'native-field-limit')

    def test_malformed_frames_reject_before_status_interpretation(self):
        valid = response()
        wrong_lengths = valid[:100]+struct.pack('>4I',0,0,0,0)+valid[116:]
        bad_hits = valid[:65]+b'\x02\x01\x00'+valid[68:]
        for tag,payload,problem in (
            (b'S',valid,'invalid-native-prefix'),
            (b'N',valid[:115],'invalid-native-prefix'),
            (b'N',b'\x02'+valid[1:],'invalid-native-prefix'),
            (b'N',valid[:-1],'inconsistent-native-lengths'),
            (b'N',valid+b'x','inconsistent-native-lengths'),
            (b'N',wrong_lengths,'inconsistent-native-lengths'),
            (b'N',bad_hits,'invalid-native-hit'),
        ):
            with self.subTest(problem=problem,tag=tag,size=len(payload)):
                self.assertEqual(self.failure(payload,tag)['problem'],problem)

    def test_success_tuple_remains_unchanged(self):
        artifact = b'complete-native-artifact'
        diagnostics = b'compiler note\n'
        actual = measurement.decode(b'N',response(status=0,reason=b'',
                                    diagnostics=diagnostics,artifact=artifact))
        self.assertEqual(actual,(artifact,(1,0,0),(0,81,0,0),'captured-context','compiler note\n'))

    def test_pipeline_failure_keeps_caller_and_never_publishes(self):
        client = mock.Mock()
        client.request.side_effect = [(b'S',b'update-fixture'),(b'N',response())]
        client.decode_update.return_value = {'status':0,'published':(7,11),'code':b'checked-C'}
        source = Path('/literal/helpers-250/slim.project')
        with mock.patch.object(measurement.native,'publish') as publish, \
                mock.patch.object(measurement.subprocess,'run') as run, \
                self.assertRaises(measurement.NativeResponseFailure) as caught:
            measurement.pipeline(client,source,0,Path('/literal/program'),b'checked-C',b'0\n',{},
                                 case='helpers-250',sample=2,operation='epoch-cold')
        publish.assert_not_called()
        run.assert_not_called()
        facts = caught.exception.args[0]
        self.assertEqual({key:facts[key] for key in ('case','sample','operation','workers','source')},
                         {'case':'helpers-250','sample':2,'operation':'epoch-cold','workers':0,
                          'source':'/literal/helpers-250/slim.project'})
        self.assertEqual(facts['failure']['status'],3)
        self.assertEqual(facts['failure']['diagnostics'],b'')
        self.assertEqual(facts['failure']['accepted_revision'],(7,11))
        self.assertEqual(facts['failure']['accepted_code_bytes'],len(b'checked-C'))
        self.assertEqual(facts['failure']['accepted_code_sha256'],hashlib.sha256(b'checked-C').hexdigest())


if __name__ == '__main__':
    unittest.main()

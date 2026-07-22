/* generated deterministically by self-hosted slimc */
#include "slim_rt.h"
#include <string.h>

typedef struct Slim_type_cache_Key Slim_type_cache_Key;
typedef struct Slim_type_cache_Number Slim_type_cache_Number;
typedef struct Slim_type_cache_Probe Slim_type_cache_Probe;
typedef struct Slim_type_check_Diagnostic Slim_type_check_Diagnostic;
typedef struct Slim_type_ir_DeclarationKind Slim_type_ir_DeclarationKind;
typedef struct Slim_type_ir_Declaration Slim_type_ir_Declaration;
typedef struct Slim_type_memory_ValuePlan Slim_type_memory_ValuePlan;
typedef struct Slim_type_memory_AllocationPlan Slim_type_memory_AllocationPlan;
typedef struct Slim_type_memory_DestructionPlan Slim_type_memory_DestructionPlan;
typedef struct Slim_type_memory_FunctionPlan Slim_type_memory_FunctionPlan;
typedef struct Slim_type_memory_Plan Slim_type_memory_Plan;
typedef struct Slim_type_project_LoadedModule Slim_type_project_LoadedModule;
typedef struct Slim_type_query_Snapshot Slim_type_query_Snapshot;
typedef struct Slim_type_query_Work Slim_type_query_Work;
typedef struct Slim_type_query_Dependency Slim_type_query_Dependency;
typedef struct Slim_type_query_State Slim_type_query_State;
typedef struct Slim_type_scheduler_Task Slim_type_scheduler_Task;
typedef struct Slim_type_scheduler_Batch Slim_type_scheduler_Batch;
typedef struct Slim_type_scheduler_Schedule Slim_type_scheduler_Schedule;
typedef struct Slim_type_syntax_Token Slim_type_syntax_Token;

struct Slim_type_cache_Key {
bool slim_field_valid;
SlimBytes slim_field_bytes;
};

struct Slim_type_cache_Number {
bool slim_field_valid;
int64_t slim_field_value;
int64_t slim_field_next;
};

struct Slim_type_cache_Probe {
bool slim_field_valid;
int64_t slim_field_artifact_start;
int64_t slim_field_artifact_end;
};

struct Slim_type_check_Diagnostic {
SlimBytes slim_field_code;
int64_t slim_field_start;
int64_t slim_field_end;
};

#define SLIM_TAG_ir_DeclarationKind_Function ((int32_t)0)
#define SLIM_TAG_ir_DeclarationKind_Record ((int32_t)1)
#define SLIM_TAG_ir_DeclarationKind_Variant ((int32_t)2)
struct Slim_type_ir_DeclarationKind {
int32_t tag;
union {
uint8_t slim_unused;
} data;
};

struct Slim_type_ir_Declaration {
Slim_type_ir_DeclarationKind slim_field_kind;
int64_t slim_field_form;
int64_t slim_field_name;
int64_t slim_field_next;
};

struct Slim_type_memory_ValuePlan {
int64_t slim_field_name;
int64_t slim_field_type_index;
int64_t slim_field_storage;
bool slim_field_borrowed;
int64_t slim_field_declared;
int64_t slim_field_last_use;
int64_t slim_field_escape;
};

struct Slim_type_memory_AllocationPlan {
int64_t slim_field_site;
int64_t slim_field_region;
};

struct Slim_type_memory_DestructionPlan {
int64_t slim_field_region;
int64_t slim_field_end;
bool slim_field_reverse;
};

struct Slim_type_memory_FunctionPlan {
int64_t slim_field_function;
int64_t slim_field_return_storage;
bool slim_field_local_region;
bool slim_field_liveness_precise;
SlimVec slim_field_values;
SlimVec slim_field_allocations;
SlimVec slim_field_destructions;
int64_t slim_field_escaping_values;
};

struct Slim_type_memory_Plan {
SlimVec slim_field_functions;
};

struct Slim_type_project_LoadedModule {
int64_t slim_field_manifest;
int64_t slim_field_name;
int64_t slim_field_path;
SlimBytes slim_field_source;
int64_t slim_field_root;
};

struct Slim_type_query_Snapshot {
SlimBytes slim_field_module;
int64_t slim_field_kind;
SlimBytes slim_field_name;
SlimBytes slim_field_source;
int64_t slim_field_form;
int64_t slim_field_next;
int64_t slim_field_start;
int64_t slim_field_interface_end;
int64_t slim_field_end;
};

struct Slim_type_query_Work {
int64_t slim_field_parsed;
int64_t slim_field_lowered;
int64_t slim_field_checked;
int64_t slim_field_generated;
};

struct Slim_type_query_Dependency {
int64_t slim_field_consumer;
int64_t slim_field_provider;
};

struct Slim_type_query_State {
SlimVec slim_field_snapshots;
SlimVec slim_field_dependencies;
};

struct Slim_type_scheduler_Task {
int64_t slim_field_module;
int64_t slim_field_layer;
};

struct Slim_type_scheduler_Batch {
int64_t slim_field_start;
int64_t slim_field_end;
};

struct Slim_type_scheduler_Schedule {
bool slim_field_valid;
int64_t slim_field_workers;
SlimVec slim_field_tasks;
SlimVec slim_field_batches;
};

struct Slim_type_syntax_Token {
int64_t slim_field_kind;
int64_t slim_field_start;
int64_t slim_field_end;
};

static Slim_type_cache_Number slim_fn_cache_valid_number(int64_t slim_v_value, int64_t slim_v_next, SlimRegion *slim_region);
static Slim_type_cache_Number slim_fn_cache_invalid_number(int64_t slim_v_next, SlimRegion *slim_region);
static Slim_type_cache_Key slim_fn_cache_invalid_key(SlimRegion *slim_region);
static Slim_type_cache_Key slim_fn_cache_make_key(bool slim_v_flag, SlimBytes slim_v_content, SlimRegion *slim_region);
static SlimUnit slim_fn_cache_append_u64(int64_t slim_v_value, int64_t slim_v_divisor, int64_t slim_v_remaining, SlimVec * slim_v_output, SlimRegion *slim_region);
static Slim_type_cache_Number slim_fn_cache_read_bounded_u64_from(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_remaining, int64_t slim_v_limit, int64_t slim_v_value, SlimRegion *slim_region);
static Slim_type_cache_Number slim_fn_cache_read_bounded_u64(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_limit, SlimRegion *slim_region);
static int64_t slim_fn_cache_weighted_checksum(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, int64_t slim_v_sum, SlimRegion *slim_region);
static bool slim_fn_cache_append_module_key(SlimBytes slim_v_manifest_path, SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region);
static Slim_type_cache_Key slim_fn_cache_project_key(SlimBytes slim_v_manifest_path, SlimRegion *slim_region);
static SlimBytes slim_fn_cache_encode(SlimBytes slim_v_key, SlimBytes slim_v_artifact, SlimRegion *slim_region);
static Slim_type_cache_Probe slim_fn_cache_invalid_probe(SlimRegion *slim_region);
static Slim_type_cache_Probe slim_fn_cache_probe(SlimBytes slim_v_cache, SlimBytes slim_v_key, SlimRegion *slim_region);
static int64_t slim_fn_cache_emit_hit(SlimBytes slim_v_cache, Slim_type_cache_Probe slim_v_cache_probe, SlimRegion *slim_region);
static int64_t slim_fn_cache_emit_miss(SlimBytes slim_v_manifest_path, SlimBytes slim_v_key, SlimRegion *slim_region);
static int64_t slim_fn_cache_run(SlimBytes slim_v_manifest_path, SlimBytes slim_v_cache_path, SlimRegion *slim_region);
static bool slim_fn_check_parameter_has_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_parameter, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region);
static bool slim_fn_check_prior_let_has_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region);
static bool slim_fn_check_match_name_is_known(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body_start, int64_t slim_v_name, SlimRegion *slim_region);
static int64_t slim_fn_check_report_boolean_match_arms(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body_start, int64_t slim_v_expected, int64_t slim_v_cursor, bool slim_v_seen_true, bool slim_v_seen_false, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_check_report_boolean_match_span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body_start, int64_t slim_v_expected, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_check_report_boolean_match_items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, int64_t slim_v_index, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_check_check_path_all(SlimBytes slim_v_path, SlimRegion *slim_region);
static int64_t slim_fn_check_recur_parameter_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_parameter, SlimRegion *slim_region);
static int64_t slim_fn_check_report_recur_arguments(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_argument, int64_t slim_v_parameter, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_check_report_recur_span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_check_report_recur_items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, int64_t slim_v_index, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_check_check_path_recur(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region);
static bool slim_fn_check_ownership_type_is_copy(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_type_index, SlimRegion *slim_region);
static int64_t slim_fn_check_ownership_parameter_type(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_parameter, SlimRegion *slim_region);
static bool slim_fn_check_moved_has(SlimBytes slim_v_source, SlimVec * slim_v_moved, int64_t slim_v_index, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region);
static int64_t slim_fn_check_report_owned_arguments(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_current_params, int64_t slim_v_argument, int64_t slim_v_parameter, SlimVec * slim_v_moved, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_check_report_builtin_move(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_current_params, int64_t slim_v_callee, int64_t slim_v_argument, SlimVec * slim_v_moved, SlimRegion *slim_region);
static int64_t slim_fn_check_report_moves_span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_current_params, int64_t slim_v_cursor, int64_t slim_v_end, SlimVec * slim_v_moved, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_check_report_inout_return(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_return_type, int64_t slim_v_body, SlimRegion *slim_region);
static int64_t slim_fn_check_report_move_items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, SlimVec * slim_v_declarations, int64_t slim_v_index, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_check_check_path_moves(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region);
static bool slim_fn_check_parameter_is_inout(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_parameter, SlimRegion *slim_region);
static int64_t slim_fn_check_find_inout_temporary_args(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_argument, int64_t slim_v_parameter, SlimRegion *slim_region);
static int64_t slim_fn_check_find_inout_temporary_span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_cursor, int64_t slim_v_end, SlimRegion *slim_region);
static int64_t slim_fn_check_find_inout_temporary_items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimRegion *slim_region);
static bool slim_fn_check_prior_inout_alias(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_argument, int64_t slim_v_parameter, int64_t slim_v_current_argument, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region);
static int64_t slim_fn_check_find_inout_alias_args(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_first_argument, int64_t slim_v_first_parameter, int64_t slim_v_argument, int64_t slim_v_parameter, SlimRegion *slim_region);
static int64_t slim_fn_check_find_inout_alias_span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_cursor, int64_t slim_v_end, SlimRegion *slim_region);
static int64_t slim_fn_check_find_inout_alias_items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_check_check_path_inout(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region);
static bool slim_fn_check_boolean_arms_have(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimBytes slim_v_pattern, SlimRegion *slim_region);
static bool slim_fn_check_boolean_match_incomplete(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, SlimRegion *slim_region);
static int64_t slim_fn_check_find_nonexhaustive_in_span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_end, SlimRegion *slim_region);
static int64_t slim_fn_check_find_nonexhaustive_items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_check_check_path_exhaustive(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region);
static bool slim_fn_check_effect_list_has(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_effect_code, SlimRegion *slim_region);
static bool slim_fn_check_builtin_requires_effect(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, int64_t slim_v_effect_code, SlimRegion *slim_region);
static int64_t slim_fn_check_find_function_effects(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_callee_start, int64_t slim_v_callee_end, SlimRegion *slim_region);
static bool slim_fn_check_call_requires_effect(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_callee, int64_t slim_v_effect_code, SlimRegion *slim_region);
static bool slim_fn_check_call_missing_effect(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_declared_effects, int64_t slim_v_callee, int64_t slim_v_effect_code, SlimRegion *slim_region);
static int64_t slim_fn_check_find_missing_effect_in_span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_declared_effects, int64_t slim_v_cursor, int64_t slim_v_end, SlimRegion *slim_region);
static int64_t slim_fn_check_find_missing_effect_items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_check_check_path_effects(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region);
static int64_t slim_fn_check_find_overqualified(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region);
static int64_t slim_fn_check_check_path_project(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region);
static int64_t slim_fn_check_result_expr(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, SlimRegion *slim_region);
static int64_t slim_fn_check_atom_type_kind(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, SlimRegion *slim_region);
static int64_t slim_fn_check_find_type_mismatch_items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_check_check_path_typed(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region);
static int64_t slim_fn_check_find_unterminated(SlimVec * slim_v_tokens, int64_t slim_v_index, int64_t slim_v_length, int64_t slim_v_depth, int64_t slim_v_form_start, SlimRegion *slim_region);
static SlimUnit slim_fn_check_append_diagnostic_identity(Slim_type_check_Diagnostic slim_v_diagnostic, SlimVec * slim_v_output, SlimRegion *slim_region);
static int64_t slim_fn_check_check_path(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region);
static int64_t slim_fn_check_last_form_in_list(SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_previous, SlimRegion *slim_region);
static bool slim_fn_check_known_expr_head(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_head, SlimRegion *slim_region);
static int64_t slim_fn_check_find_unknown_pair(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_value, int64_t slim_v_body, SlimRegion *slim_region);
static int64_t slim_fn_check_find_unknown_control(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, int64_t slim_v_head, SlimRegion *slim_region);
static int64_t slim_fn_check_find_unknown_data(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, int64_t slim_v_head, SlimRegion *slim_region);
static int64_t slim_fn_check_find_unknown_known(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, int64_t slim_v_head, SlimRegion *slim_region);
static int64_t slim_fn_check_find_unknown_expr(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, SlimRegion *slim_region);
static int64_t slim_fn_check_find_unknown_forms(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region);
static int64_t slim_fn_check_find_unknown_fields(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region);
static int64_t slim_fn_check_find_unknown_arms(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region);
static int64_t slim_fn_check_find_unknown_items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_check_report_diagnostic(SlimBytes slim_v_code, int64_t slim_v_start, int64_t slim_v_end, SlimRegion *slim_region);
static int64_t slim_fn_check_check_path_full(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_value_address(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_value, SlimVec * slim_v_output, SlimRegion *slim_region);
static bool slim_fn_codegen_emit_vec_set_call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region);
static bool slim_fn_codegen_emit_arena_call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_type, SlimVec * slim_v_output, SlimRegion *slim_region);
static bool slim_fn_codegen_emit_collection_call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_type, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_let_full(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_type, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_match_full(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_type, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_set_full(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_type, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_expr_full(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_type, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_source_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_function_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_named_type_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_field_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_case_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_tag_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_variant, int64_t slim_v_case_name, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_type(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_forward_types(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_record_fields(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_record(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_item, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_variant_tags(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_variant_name, int64_t slim_v_cursor, int64_t slim_v_tag, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_variant_payload_fields(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_field, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_variant_payloads(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_variant(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_item, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_data_items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_binding_value(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_name, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_binding_address(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_name, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_value(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_value, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimBytes slim_fn_codegen_runtime_math_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region);
static SlimBytes slim_fn_codegen_runtime_bytes_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region);
static SlimBytes slim_fn_codegen_runtime_io_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region);
static SlimBytes slim_fn_codegen_runtime_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_value_arguments(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_user_arguments(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_current_params, int64_t slim_v_argument, int64_t slim_v_callee_param, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_parameters(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_prototype(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_item, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_prototypes(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region);
static bool slim_fn_codegen_contains_atom(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_end, SlimBytes slim_v_text, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_parameter_voids(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_temp_name(int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_destination(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_recur_assignments(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_param, int64_t slim_v_argument, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_runtime_assignment(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimBytes slim_v_runtime, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_binary_operator(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimBytes slim_v_operator, SlimVec * slim_v_output, SlimRegion *slim_region);
static bool slim_fn_codegen_emit_comparison(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region);
static bool slim_fn_codegen_emit_boolean_call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region);
static bool slim_fn_codegen_emit_u8_conversion(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region);
static bool slim_fn_codegen_emit_vector_call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_type, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_user_call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_params, int64_t slim_v_call_form, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_type, SlimVec * slim_v_output, SlimRegion *slim_region);
static int64_t slim_fn_codegen_find_record_item(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region);
static int64_t slim_fn_codegen_find_record_field(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_make_bindings(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_params, int64_t slim_v_definition, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_make_fields(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_make(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_get(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_case_bindings(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_params, int64_t slim_v_cursor, int64_t slim_v_payload_type, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_case_payload(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_cursor, int64_t slim_v_field, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_case_value(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region);
static int64_t slim_fn_codegen_find_parameter_type(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region);
static int64_t slim_fn_codegen_find_variant_item(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region);
static int64_t slim_fn_codegen_find_variant_case(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_variant_bindings(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_value, int64_t slim_v_case_name, int64_t slim_v_bindings, int64_t slim_v_payload_type, int64_t slim_v_field, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_variant_arms(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_params, int64_t slim_v_value, int64_t slim_v_variant_name, int64_t slim_v_variant_cases, int64_t slim_v_cursor, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_type, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_boolean_match(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_params, int64_t slim_v_value, int64_t slim_v_first_arm, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_type, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_variant_match(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_params, int64_t slim_v_value, int64_t slim_v_first_arm, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_type, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_atom_assignment(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_value, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_function(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_item, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_functions(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_emit_program(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_output, SlimRegion *slim_region);
static bool slim_fn_codegen_emit_read_file_call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region);
static int64_t slim_fn_compiler_check_input_path(SlimBytes slim_v_path, SlimRegion *slim_region);
static SlimUnit slim_fn_compiler_format_tokens(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, int64_t slim_v_length, int64_t slim_v_previous_kind, SlimVec * slim_v_output, SlimRegion *slim_region);
static int64_t slim_fn_compiler_format_path(SlimBytes slim_v_path, SlimRegion *slim_region);
static int64_t slim_fn_compiler_compile_path(SlimBytes slim_v_path, SlimRegion *slim_region);
static int64_t slim_fn_compiler_run_compile(SlimVec * slim_v_args, int64_t slim_v_arg_count, SlimBytes slim_v_path, SlimRegion *slim_region);
static int64_t slim_fn_compiler_run_cache(SlimVec * slim_v_args, int64_t slim_v_arg_count, SlimRegion *slim_region);
static int64_t slim_fn_compiler_run_session(SlimVec * slim_v_args, int64_t slim_v_arg_count, SlimRegion *slim_region);
static int64_t slim_fn_compiler_run(SlimVec slim_v_args, SlimRegion *slim_region);
static int64_t slim_fn_main(SlimVec slim_v_args, SlimRegion *slim_region);
static int64_t slim_fn_memory_storage_join(int64_t slim_v_left, int64_t slim_v_right, SlimRegion *slim_region);
static int64_t slim_fn_memory_find_type_item(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region);
static int64_t slim_fn_memory_record_storage(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_cursor, int64_t slim_v_depth, int64_t slim_v_current, SlimRegion *slim_region);
static int64_t slim_fn_memory_payload_storage(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_cursor, int64_t slim_v_depth, int64_t slim_v_current, SlimRegion *slim_region);
static int64_t slim_fn_memory_variant_storage(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_cursor, int64_t slim_v_depth, int64_t slim_v_current, SlimRegion *slim_region);
static int64_t slim_fn_memory_type_storage_kind_from(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_type_index, int64_t slim_v_depth, SlimRegion *slim_region);
static int64_t slim_fn_memory_type_storage_kind(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_type_index, SlimRegion *slim_region);
static bool slim_fn_memory_parameters_have_inout(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region);
static bool slim_fn_memory_function_uses_local_region(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_function, SlimRegion *slim_region);
static bool slim_fn_memory_effect_list_has_alloc(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region);
static bool slim_fn_memory_params_have_alloc_effect(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, SlimRegion *slim_region);
static bool slim_fn_memory_function_has_alloc_effect(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_function, SlimRegion *slim_region);
static int64_t slim_fn_memory_last_named_use(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_name_start, int64_t slim_v_name_end, int64_t slim_v_last, SlimRegion *slim_region);
static bool slim_fn_memory_span_has_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region);
static int64_t slim_fn_memory_result_expression(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, SlimRegion *slim_region);
static bool slim_fn_memory_escapes_to_inout(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region);
static int64_t slim_fn_memory_escape_kind(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body, int64_t slim_v_body_end, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region);
static SlimUnit slim_fn_memory_append_value_plan(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body, int64_t slim_v_body_end, int64_t slim_v_name, int64_t slim_v_type_index, bool slim_v_borrowed, int64_t slim_v_declared, SlimVec * slim_v_values, SlimRegion *slim_region);
static SlimUnit slim_fn_memory_append_parameter_plans(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body, int64_t slim_v_body_end, int64_t slim_v_cursor, SlimVec * slim_v_values, SlimRegion *slim_region);
static SlimUnit slim_fn_memory_append_local_plans(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body, int64_t slim_v_body_end, int64_t slim_v_cursor, SlimVec * slim_v_values, SlimRegion *slim_region);
static SlimUnit slim_fn_memory_append_allocation_plans(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_region, SlimVec * slim_v_allocations, SlimRegion *slim_region);
static int64_t slim_fn_memory_conservative_escape(int64_t slim_v_return_storage, bool slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_memory_finalize_value_plans(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body, int64_t slim_v_body_end, int64_t slim_v_return_storage, bool slim_v_output, SlimVec * slim_v_values, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_memory_count_escaping_values(SlimVec * slim_v_values, int64_t slim_v_index, int64_t slim_v_count, SlimRegion *slim_region);
static Slim_type_memory_FunctionPlan slim_fn_memory_build_function_plan(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_function, SlimRegion *slim_region);
static SlimUnit slim_fn_memory_append_function_plans(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimVec * slim_v_functions, SlimRegion *slim_region);
static Slim_type_memory_Plan slim_fn_memory_analyze(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region);
static bool slim_fn_project_cross_span_chars_equal(SlimBytes slim_v_left, int64_t slim_v_left_start, SlimBytes slim_v_right, int64_t slim_v_right_start, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region);
static bool slim_fn_project_cross_spans_equal(SlimBytes slim_v_left, int64_t slim_v_left_start, int64_t slim_v_left_end, SlimBytes slim_v_right, int64_t slim_v_right_start, int64_t slim_v_right_end, SlimRegion *slim_region);
static int64_t slim_fn_project_last_path_slash(SlimBytes slim_v_path, int64_t slim_v_index, int64_t slim_v_length, int64_t slim_v_last, SlimRegion *slim_region);
static SlimBytes slim_fn_project_project_module_path(SlimBytes slim_v_manifest_path, SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_path_token, SlimRegion *slim_region);
static bool slim_fn_project_declaration_has_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name, SlimRegion *slim_region);
static SlimUnit slim_fn_project_append_slash_replaced(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_project_append_project_atom(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_module_items, bool slim_v_entry, int64_t slim_v_atom, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_project_append_project_tokens(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_module_items, bool slim_v_entry, int64_t slim_v_index, int64_t slim_v_end, int64_t slim_v_previous_kind, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_project_append_project_modules(SlimBytes slim_v_manifest_path, SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_entry_name, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimBytes slim_fn_project_flatten_project(SlimBytes slim_v_path, SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimRegion *slim_region);
static bool slim_fn_project_path_is_project(SlimBytes slim_v_path, SlimRegion *slim_region);
static SlimBytes slim_fn_project_generate_project_path(SlimBytes slim_v_path, SlimRegion *slim_region);
static int64_t slim_fn_project_compile_project_path_jobs(SlimBytes slim_v_path, int64_t slim_v_jobs, SlimRegion *slim_region);
static int64_t slim_fn_project_compile_project_path(SlimBytes slim_v_path, SlimRegion *slim_region);
static SlimBytes slim_fn_project_token_bytes(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_project_report_project_diagnostic(SlimBytes slim_v_code, SlimBytes slim_v_module_name, int64_t slim_v_start, int64_t slim_v_end, SlimRegion *slim_region);
static bool slim_fn_project_imports_has_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region);
static int64_t slim_fn_project_find_manifest_module(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region);
static bool slim_fn_project_module_imports_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_module_name, int64_t slim_v_import_start, int64_t slim_v_import_end, SlimRegion *slim_region);
static bool slim_fn_project_module_cycle_imports(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_module_name, int64_t slim_v_cursor, SlimRegion *slim_region);
static int64_t slim_fn_project_find_reciprocal_cycle(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_cursor, SlimRegion *slim_region);
static int64_t slim_fn_project_find_span_slash(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, SlimRegion *slim_region);
static bool slim_fn_project_exports_has_segment(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_cursor, SlimBytes slim_v_module_source, int64_t slim_v_segment_start, int64_t slim_v_segment_end, SlimRegion *slim_region);
static int64_t slim_fn_project_check_project_path(SlimBytes slim_v_path, SlimRegion *slim_region);
static SlimUnit slim_fn_project_append_interface_type(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_type_index, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_project_append_interface_parameters(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_project_append_interface_effects(SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region);
static int64_t slim_fn_project_find_export_declaration(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_export_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_cursor, SlimRegion *slim_region);
static SlimUnit slim_fn_project_append_interface_fields(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_project_append_interface_case_types(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_project_append_interface_cases(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_project_append_interface_declaration(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_item, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_project_append_interface_exports(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_module_name, int64_t slim_v_cursor, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_module_items, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_project_append_project_interfaces(SlimBytes slim_v_manifest_path, SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region);
static int64_t slim_fn_project_interfaces_path(SlimBytes slim_v_path, SlimRegion *slim_region);
static bool slim_fn_project_span_less_chars(SlimBytes slim_v_source, int64_t slim_v_left, int64_t slim_v_left_end, int64_t slim_v_right, int64_t slim_v_right_end, SlimRegion *slim_region);
static bool slim_fn_project_span_less(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_left, int64_t slim_v_right, SlimRegion *slim_region);
static int64_t slim_fn_project_find_unsorted_module(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_previous, SlimRegion *slim_region);
static int64_t slim_fn_project_find_prior_module_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_current, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region);
static int64_t slim_fn_project_find_duplicate_module(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_cursor, SlimRegion *slim_region);
static int64_t slim_fn_project_find_prior_path(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_current, int64_t slim_v_path_start, int64_t slim_v_path_end, SlimRegion *slim_region);
static int64_t slim_fn_project_find_duplicate_path(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_cursor, SlimRegion *slim_region);
static bool slim_fn_project_path_segment_valid(SlimBytes slim_v_source, int64_t slim_v_start, int64_t slim_v_end, SlimRegion *slim_region);
static bool slim_fn_project_path_segments_valid(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, int64_t slim_v_segment_start, SlimRegion *slim_region);
static bool slim_fn_project_path_token_invalid(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_path, SlimRegion *slim_region);
static int64_t slim_fn_project_find_invalid_path(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region);
static int64_t slim_fn_project_find_self_import_in(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region);
static int64_t slim_fn_project_find_self_import(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region);
static int64_t slim_fn_project_find_unknown_import_in(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_cursor, SlimRegion *slim_region);
static int64_t slim_fn_project_find_unknown_import(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_cursor, SlimRegion *slim_region);
static int64_t slim_fn_project_report_manifest_rules(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_entry_form, int64_t slim_v_modules, SlimRegion *slim_region);
static int64_t slim_fn_project_load_project_modules(SlimBytes slim_v_manifest_path, SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_cursor, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, SlimRegion *slim_region);
static int64_t slim_fn_project_report_module_identities(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_project_find_missing_export(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_cursor, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_module_items, SlimRegion *slim_region);
static int64_t slim_fn_project_report_missing_exports(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_project_find_named_declaration(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimBytes slim_v_name, SlimRegion *slim_region);
static bool slim_fn_project_loaded_is_entry(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_entry, Slim_type_project_LoadedModule slim_v_module, SlimRegion *slim_region);
static int64_t slim_fn_project_report_project_mains(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_entry, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_project_find_loaded_segment(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_loaded, SlimBytes slim_v_reference_source, int64_t slim_v_segment_start, int64_t slim_v_segment_end, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_project_find_declaration_segment(SlimBytes slim_v_declaration_source, SlimVec * slim_v_module_tokens, int64_t slim_v_cursor, SlimBytes slim_v_reference_source, int64_t slim_v_segment_start, int64_t slim_v_segment_end, SlimRegion *slim_region);
static int64_t slim_fn_project_target_call_status(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, Slim_type_project_LoadedModule slim_v_current, int64_t slim_v_target_index, SlimBytes slim_v_reference_source, int64_t slim_v_prefix_start, int64_t slim_v_slash, int64_t slim_v_reference_end, SlimRegion *slim_region);
static int64_t slim_fn_project_qualified_call_status(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, Slim_type_project_LoadedModule slim_v_current, int64_t slim_v_callee, SlimRegion *slim_region);
static int64_t slim_fn_project_find_invalid_loaded_call(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, Slim_type_project_LoadedModule slim_v_current, int64_t slim_v_cursor, int64_t slim_v_end, SlimRegion *slim_region);
static int64_t slim_fn_project_report_qualified_calls(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_project_report_loaded_project(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_entry, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, SlimRegion *slim_region);
static int64_t slim_fn_project_report_interface_type(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_first_export, SlimBytes slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_module_items, int64_t slim_v_type_index, int64_t slim_v_diagnostic_start, int64_t slim_v_diagnostic_end, SlimRegion *slim_region);
static int64_t slim_fn_project_report_interface_bindings(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_first_export, SlimBytes slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_module_items, int64_t slim_v_cursor, int64_t slim_v_diagnostic_start, int64_t slim_v_diagnostic_end, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_project_report_interface_payload(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_first_export, SlimBytes slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_module_items, int64_t slim_v_cursor, int64_t slim_v_diagnostic_start, int64_t slim_v_diagnostic_end, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_project_report_interface_cases(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_first_export, SlimBytes slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_module_items, int64_t slim_v_cursor, int64_t slim_v_diagnostic_start, int64_t slim_v_diagnostic_end, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_project_report_export_interfaces(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_first_export, SlimBytes slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_module_items, int64_t slim_v_cursor, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_project_report_private_type_leaks_from(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, int64_t slim_v_index, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_project_report_private_type_leaks(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, SlimRegion *slim_region);
static SlimBytes slim_fn_query_copy_span(SlimBytes slim_v_source, int64_t slim_v_start, int64_t slim_v_end, SlimRegion *slim_region);
static bool slim_fn_query_bytes_equal_from(SlimBytes slim_v_left, SlimBytes slim_v_right, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region);
static bool slim_fn_query_bytes_equal(SlimBytes slim_v_left, SlimBytes slim_v_right, SlimRegion *slim_region);
static int64_t slim_fn_query_declaration_kind(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_item, SlimRegion *slim_region);
static int64_t slim_fn_query_declaration_interface_end(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_item, SlimRegion *slim_region);
static SlimUnit slim_fn_query_append_module_snapshots(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_module_tokens, Slim_type_project_LoadedModule slim_v_module, int64_t slim_v_cursor, SlimBytes slim_v_module_name, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_query_build_snapshots_from(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimVec slim_fn_query_build_snapshots(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, SlimRegion *slim_region);
static bool slim_fn_query_snapshot_identity_equal(Slim_type_query_Snapshot slim_v_left, Slim_type_query_Snapshot slim_v_right, SlimRegion *slim_region);
static bool slim_fn_query_snapshot_body_equal(Slim_type_query_Snapshot slim_v_left, Slim_type_query_Snapshot slim_v_right, SlimRegion *slim_region);
static int64_t slim_fn_query_find_snapshot(SlimVec * slim_v_snapshots, Slim_type_query_Snapshot slim_v_candidate, int64_t slim_v_index, SlimRegion *slim_region);
static SlimBytes slim_fn_query_snapshot_qualified_name(Slim_type_query_Snapshot slim_v_snapshot, SlimRegion *slim_region);
static int64_t slim_fn_query_find_provider(SlimBytes slim_v_atom, SlimVec * slim_v_snapshots, int64_t slim_v_index, SlimRegion *slim_region);
static bool slim_fn_query_dependency_exists(SlimVec * slim_v_dependencies, int64_t slim_v_consumer, int64_t slim_v_provider, int64_t slim_v_index, SlimRegion *slim_region);
static SlimUnit slim_fn_query_append_snapshot_dependencies(SlimVec * slim_v_tokens, SlimVec * slim_v_snapshots, int64_t slim_v_consumer, int64_t slim_v_cursor, int64_t slim_v_end, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_query_build_dependencies_from(SlimVec * slim_v_tokens, SlimVec * slim_v_snapshots, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimVec slim_fn_query_build_dependencies(SlimVec * slim_v_tokens, SlimVec * slim_v_snapshots, SlimRegion *slim_region);
static Slim_type_query_State slim_fn_query_build_state(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, SlimRegion *slim_region);
static bool slim_fn_query_snapshot_interface_equal(Slim_type_query_Snapshot slim_v_left, Slim_type_query_Snapshot slim_v_right, SlimRegion *slim_region);
static SlimUnit slim_fn_query_fill_flags(int64_t slim_v_length, int64_t slim_v_index, SlimVec * slim_v_flags, SlimRegion *slim_region);
static int64_t slim_fn_query_classify_changes(SlimVec * slim_v_previous, SlimVec * slim_v_current, int64_t slim_v_index, int64_t slim_v_count, SlimVec * slim_v_invalidated, SlimVec * slim_v_queue, SlimRegion *slim_region);
static SlimUnit slim_fn_query_mark_consumers(SlimVec * slim_v_dependencies, int64_t slim_v_provider, int64_t slim_v_index, SlimVec * slim_v_invalidated, SlimVec * slim_v_queue, SlimRegion *slim_region);
static SlimUnit slim_fn_query_propagate_invalidations(SlimVec * slim_v_dependencies, SlimVec * slim_v_queue, int64_t slim_v_cursor, SlimVec * slim_v_invalidated, SlimRegion *slim_region);
static int64_t slim_fn_query_count_flags(SlimVec * slim_v_flags, int64_t slim_v_index, int64_t slim_v_count, SlimRegion *slim_region);
static Slim_type_query_Work slim_fn_query_measure_update(SlimVec * slim_v_previous, SlimVec * slim_v_current, SlimVec * slim_v_dependencies, SlimRegion *slim_region);
static SlimUnit slim_fn_scheduler_collect_modules(SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_modules, SlimRegion *slim_region);
static SlimUnit slim_fn_scheduler_fill_pending(int64_t slim_v_length, int64_t slim_v_index, SlimVec * slim_v_scheduled, SlimRegion *slim_region);
static int64_t slim_fn_scheduler_find_module(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_modules, int64_t slim_v_name, int64_t slim_v_index, SlimRegion *slim_region);
static bool slim_fn_scheduler_imports_ready(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_modules, SlimVec * slim_v_scheduled, int64_t slim_v_cursor, SlimRegion *slim_region);
static SlimUnit slim_fn_scheduler_collect_ready(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_modules, SlimVec * slim_v_scheduled, int64_t slim_v_index, SlimVec * slim_v_ready, SlimRegion *slim_region);
static SlimUnit slim_fn_scheduler_append_tasks(SlimVec * slim_v_ready, int64_t slim_v_index, int64_t slim_v_layer, SlimVec * slim_v_tasks, SlimRegion *slim_region);
static SlimUnit slim_fn_scheduler_append_batches(int64_t slim_v_start, int64_t slim_v_end, int64_t slim_v_workers, SlimVec * slim_v_batches, SlimRegion *slim_region);
static SlimUnit slim_fn_scheduler_mark_ready(SlimVec * slim_v_ready, int64_t slim_v_index, SlimVec * slim_v_scheduled, SlimRegion *slim_region);
static bool slim_fn_scheduler_schedule_layers(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_modules, SlimVec * slim_v_scheduled, int64_t slim_v_completed, int64_t slim_v_layer, int64_t slim_v_workers, SlimVec * slim_v_tasks, SlimVec * slim_v_batches, SlimRegion *slim_region);
static int64_t slim_fn_scheduler_bounded_workers(int64_t slim_v_requested, int64_t slim_v_module_count, SlimRegion *slim_region);
static Slim_type_scheduler_Schedule slim_fn_scheduler_make_schedule(bool slim_v_valid, int64_t slim_v_workers, SlimVec slim_v_tasks, SlimVec slim_v_batches, SlimRegion *slim_region);
static Slim_type_scheduler_Schedule slim_fn_scheduler_plan(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_requested, SlimRegion *slim_region);
static int64_t slim_fn_scheduler_parse_jobs_from(SlimBytes slim_v_input, int64_t slim_v_index, int64_t slim_v_length, int64_t slim_v_value, SlimRegion *slim_region);
static int64_t slim_fn_scheduler_parse_jobs(SlimBytes slim_v_input, SlimRegion *slim_region);
static Slim_type_query_State slim_fn_session_state_for_path(SlimBytes slim_v_path, SlimRegion *slim_region);
static int64_t slim_fn_session_print_work(Slim_type_query_Work slim_v_work, SlimRegion *slim_region);
static int64_t slim_fn_session_run(SlimBytes slim_v_initial_path, SlimBytes slim_v_updated_path, SlimRegion *slim_region);
static int64_t slim_fn_session_run_recovery(SlimBytes slim_v_initial_path, SlimBytes slim_v_invalid_path, SlimBytes slim_v_recovered_path, SlimRegion *slim_region);
static int64_t slim_fn_syntax_token_kind(SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_syntax_token_start(SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_syntax_token_end(SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region);
static bool slim_fn_syntax_span_equal_chars(SlimBytes slim_v_source, int64_t slim_v_start, SlimBytes slim_v_text, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region);
static bool slim_fn_syntax_span_equal(SlimBytes slim_v_source, int64_t slim_v_start, int64_t slim_v_end, SlimBytes slim_v_text, SlimRegion *slim_region);
static bool slim_fn_syntax_spans_equal_chars(SlimBytes slim_v_source, int64_t slim_v_left_start, int64_t slim_v_right_start, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region);
static bool slim_fn_syntax_spans_equal(SlimBytes slim_v_source, int64_t slim_v_left_start, int64_t slim_v_left_end, int64_t slim_v_right_start, int64_t slim_v_right_end, SlimRegion *slim_region);
static bool slim_fn_syntax_is_delimiter(int64_t slim_v_code, SlimRegion *slim_region);
static int64_t slim_fn_syntax_scan_atom(SlimBytes slim_v_input, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region);
static int64_t slim_fn_syntax_scan_string(SlimBytes slim_v_input, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region);
static int64_t slim_fn_syntax_scan_comment(SlimBytes slim_v_input, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region);
static SlimUnit slim_fn_syntax_push_token(SlimVec * slim_v_tokens, int64_t slim_v_kind, int64_t slim_v_start, int64_t slim_v_end, SlimRegion *slim_region);
static SlimUnit slim_fn_syntax_lex(SlimBytes slim_v_input, int64_t slim_v_index, SlimVec * slim_v_tokens, SlimRegion *slim_region);
static bool slim_fn_syntax_token_equal(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimBytes slim_v_text, SlimRegion *slim_region);
static int64_t slim_fn_syntax_skip_nested(SlimVec * slim_v_tokens, int64_t slim_v_index, int64_t slim_v_depth, SlimRegion *slim_region);
static int64_t slim_fn_syntax_skip_form(SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region);
static bool slim_fn_syntax_item_is(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_item, SlimBytes slim_v_text, SlimRegion *slim_region);
static bool slim_fn_syntax_binding_is_inout(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_cursor, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region);
static bool slim_fn_syntax_name_is_inout(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_name, SlimRegion *slim_region);
static bool slim_fn_syntax_first_byte_is_number(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_syntax_find_function_params(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_callee_start, int64_t slim_v_callee_end, SlimRegion *slim_region);
static int64_t slim_fn_syntax_span_count_byte(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, int64_t slim_v_code, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_syntax_scalar_type_kind(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_type_index, SlimRegion *slim_region);
static Slim_type_ir_DeclarationKind slim_fn_syntax_declaration_kind(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_form, SlimRegion *slim_region);
static SlimUnit slim_fn_syntax_index_declarations_from(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_declarations, SlimRegion *slim_region);
static SlimVec slim_fn_syntax_index_declarations(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimRegion *slim_region);
static SlimUnit slim_fn_text_append_span(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_text_append_text_chars(SlimBytes slim_v_text, int64_t slim_v_index, int64_t slim_v_length, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_text_append_text(SlimVec * slim_v_output, SlimBytes slim_v_text, SlimRegion *slim_region);
static SlimUnit slim_fn_text_append_nonnegative_i64(int64_t slim_v_value, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_text_append_token(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region);

static Slim_type_cache_Number slim_fn_cache_valid_number(int64_t slim_v_value, int64_t slim_v_next, SlimRegion *slim_region) {
(void)slim_v_value;
(void)slim_v_next;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
Slim_type_cache_Number slim_result = {0};
{
bool slim_t_73 = {0};
slim_t_73 = true;
int64_t slim_t_77 = {0};
slim_t_77 = slim_v_value;
int64_t slim_t_81 = {0};
slim_t_81 = slim_v_next;
slim_result = (Slim_type_cache_Number){.slim_field_valid = slim_t_73, .slim_field_value = slim_t_77, .slim_field_next = slim_t_81};
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static Slim_type_cache_Number slim_fn_cache_invalid_number(int64_t slim_v_next, SlimRegion *slim_region) {
(void)slim_v_next;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
Slim_type_cache_Number slim_result = {0};
{
bool slim_t_103 = {0};
slim_t_103 = false;
int64_t slim_t_107 = {0};
slim_t_107 = INT64_C(0);
int64_t slim_t_111 = {0};
slim_t_111 = slim_v_next;
slim_result = (Slim_type_cache_Number){.slim_field_valid = slim_t_103, .slim_field_value = slim_t_107, .slim_field_next = slim_t_111};
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static Slim_type_cache_Key slim_fn_cache_invalid_key(SlimRegion *slim_region) {
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
Slim_type_cache_Key slim_result = {0};
{
bool slim_t_129 = {0};
slim_t_129 = false;
SlimBytes slim_t_133 = {0};
slim_t_133 = slim_bytes_static((const uint8_t *)"", (int64_t)(sizeof("") - 1));
slim_result = (Slim_type_cache_Key){.slim_field_valid = slim_t_129, .slim_field_bytes = slim_t_133};
}
return slim_result;
}

static Slim_type_cache_Key slim_fn_cache_make_key(bool slim_v_flag, SlimBytes slim_v_content, SlimRegion *slim_region) {
(void)slim_v_flag;
(void)slim_v_content;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
Slim_type_cache_Key slim_result = {0};
{
bool slim_t_159 = {0};
slim_t_159 = slim_v_flag;
SlimBytes slim_t_163 = {0};
slim_t_163 = slim_v_content;
slim_result = (Slim_type_cache_Key){.slim_field_valid = slim_t_159, .slim_field_bytes = slim_t_163};
}
return slim_result;
}

static SlimUnit slim_fn_cache_append_u64(int64_t slim_v_value, int64_t slim_v_divisor, int64_t slim_v_remaining, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_value;
(void)slim_v_divisor;
(void)slim_v_remaining;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_remaining == INT64_C(0);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_quotient = {0};
slim_v_quotient = slim_i64_div(slim_v_value, slim_v_divisor);
(void)slim_v_quotient;
{
int64_t slim_v_code = {0};
slim_v_code = slim_i64_rem(slim_v_quotient, INT64_C(256));
(void)slim_v_code;
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_i64_to_u8(slim_v_code);
(void)slim_v_byte;
{
SlimUnit slim_v_pushed = {0};
if (!slim_vec_push(slim_v_output, &(slim_v_byte))) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
(void)slim_v_pushed;
{
int64_t slim_v_next_divisor = {0};
slim_v_next_divisor = slim_i64_div(slim_v_divisor, INT64_C(256));
(void)slim_v_next_divisor;
{
int64_t slim_v_next_remaining = {0};
slim_v_next_remaining = slim_i64_sub(slim_v_remaining, INT64_C(1));
(void)slim_v_next_remaining;
slim_v_divisor = slim_v_next_divisor;
slim_v_remaining = slim_v_next_remaining;
goto slim_recur;
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static Slim_type_cache_Number slim_fn_cache_read_bounded_u64_from(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_remaining, int64_t slim_v_limit, int64_t slim_v_value, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_index;
(void)slim_v_remaining;
(void)slim_v_limit;
(void)slim_v_value;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
Slim_type_cache_Number slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_remaining == INT64_C(0);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_fn_cache_valid_number(slim_v_value, slim_v_index, slim_allocation_region);
}
else {
{
int64_t slim_v_length = {0};
slim_v_length = slim_bytes_len(slim_v_source);
(void)slim_v_length;
{
bool slim_v_available = {0};
slim_v_available = slim_v_index < slim_v_length;
(void)slim_v_available;
if (!slim_v_available) {
slim_result = slim_fn_cache_invalid_number(slim_v_index, slim_allocation_region);
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_source, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
int64_t slim_v_maximum_prefix = {0};
slim_v_maximum_prefix = slim_i64_div(slim_v_limit, INT64_C(256));
(void)slim_v_maximum_prefix;
{
bool slim_v_below = {0};
slim_v_below = slim_v_value < slim_v_maximum_prefix;
(void)slim_v_below;
{
bool slim_v_equal = {0};
slim_v_equal = slim_v_value == slim_v_maximum_prefix;
(void)slim_v_equal;
{
int64_t slim_v_maximum_byte = {0};
slim_v_maximum_byte = slim_i64_rem(slim_v_limit, INT64_C(256));
(void)slim_v_maximum_byte;
{
bool slim_v_byte_fits = {0};
slim_v_byte_fits = slim_v_code <= slim_v_maximum_byte;
(void)slim_v_byte_fits;
{
bool slim_v_equal_fits = {0};
slim_v_equal_fits = slim_v_equal && slim_v_byte_fits;
(void)slim_v_equal_fits;
{
bool slim_v_fits = {0};
slim_v_fits = slim_v_below || slim_v_equal_fits;
(void)slim_v_fits;
if (!slim_v_fits) {
slim_result = slim_fn_cache_invalid_number(slim_v_index, slim_allocation_region);
}
else {
{
int64_t slim_v_shifted = {0};
slim_v_shifted = slim_i64_mul(slim_v_value, INT64_C(256));
(void)slim_v_shifted;
{
int64_t slim_v_next_value = {0};
slim_v_next_value = slim_i64_add(slim_v_shifted, slim_v_code);
(void)slim_v_next_value;
{
int64_t slim_v_next_index = {0};
slim_v_next_index = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next_index;
{
int64_t slim_v_next_remaining = {0};
slim_v_next_remaining = slim_i64_sub(slim_v_remaining, INT64_C(1));
(void)slim_v_next_remaining;
slim_v_index = slim_v_next_index;
slim_v_remaining = slim_v_next_remaining;
slim_v_value = slim_v_next_value;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static Slim_type_cache_Number slim_fn_cache_read_bounded_u64(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_limit, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_index;
(void)slim_v_limit;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
Slim_type_cache_Number slim_result = {0};
slim_result = slim_fn_cache_read_bounded_u64_from(slim_v_source, slim_v_index, INT64_C(8), slim_v_limit, INT64_C(0), slim_allocation_region);
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_cache_weighted_checksum(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, int64_t slim_v_sum, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_index;
(void)slim_v_end;
(void)slim_v_sum;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_sum;
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_source, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
int64_t slim_v_position = {0};
slim_v_position = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_position;
{
int64_t slim_v_weighted = {0};
slim_v_weighted = slim_i64_mul(slim_v_code, slim_v_position);
(void)slim_v_weighted;
{
int64_t slim_v_next_sum = {0};
slim_v_next_sum = slim_i64_add(slim_v_sum, slim_v_weighted);
(void)slim_v_next_sum;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
slim_v_sum = slim_v_next_sum;
goto slim_recur;
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_cache_append_module_key(SlimBytes slim_v_manifest_path, SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_path;
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_cursor;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_manifest_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = true;
}
else {
{
int64_t slim_v_path_token = {0};
slim_v_path_token = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_path_token;
{
SlimBytes slim_v_module_path = {0};
slim_v_module_path = slim_fn_project_project_module_path(slim_v_manifest_path, slim_v_manifest_source, slim_v_manifest_tokens, slim_v_path_token, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_path;
{
SlimVec slim_v_source_bytes = {0};
slim_v_source_bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_source_bytes;
{
bool slim_v_read = {0};
slim_v_read = slim_read_file(slim_v_module_path, &slim_v_source_bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_read;
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_bytes_freeze(slim_v_source_bytes);
(void)slim_v_source;
if (!slim_v_read) {
slim_result = false;
}
else {
{
int64_t slim_v_source_length = {0};
slim_v_source_length = slim_bytes_len(slim_v_source);
(void)slim_v_source_length;
{
int64_t slim_v_current = {0};
slim_v_current = ((*slim_v_output)).len;
(void)slim_v_current;
{
int64_t slim_v_framing_room = {0};
slim_v_framing_room = slim_i64_sub(INT64_C(67108864), slim_v_current);
(void)slim_v_framing_room;
{
bool slim_v_has_frame = {0};
slim_v_has_frame = slim_v_framing_room >= INT64_C(8);
(void)slim_v_has_frame;
{
int64_t slim_v_content_room = {0};
if (slim_v_has_frame) {
slim_v_content_room = slim_i64_sub(slim_v_framing_room, INT64_C(8));
}
else {
slim_v_content_room = INT64_C(0);
}
(void)slim_v_content_room;
{
bool slim_v_fits = {0};
slim_v_fits = slim_v_source_length <= slim_v_content_room;
(void)slim_v_fits;
{
bool slim_v_valid = {0};
slim_v_valid = slim_v_has_frame && slim_v_fits;
(void)slim_v_valid;
if (!slim_v_valid) {
slim_result = false;
}
else {
{
SlimUnit slim_v_length_bytes = {0};
slim_v_length_bytes = slim_fn_cache_append_u64(slim_v_source_length, INT64_C(72057594037927936), INT64_C(8), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_length_bytes;
{
SlimUnit slim_v_content = {0};
slim_v_content = slim_fn_text_append_text(slim_v_output, slim_v_source, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_content;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_manifest_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static Slim_type_cache_Key slim_fn_cache_project_key(SlimBytes slim_v_manifest_path, SlimRegion *slim_region) {
(void)slim_v_manifest_path;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
Slim_type_cache_Key slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_manifest_bytes = {0};
slim_v_manifest_bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_manifest_bytes;
{
bool slim_v_read = {0};
slim_v_read = slim_read_file(slim_v_manifest_path, &slim_v_manifest_bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_read;
{
SlimBytes slim_v_manifest_source = {0};
slim_v_manifest_source = slim_bytes_freeze(slim_v_manifest_bytes);
(void)slim_v_manifest_source;
if (!slim_v_read) {
slim_result = slim_fn_cache_invalid_key(slim_allocation_region);
}
else {
{
int64_t slim_v_manifest_length = {0};
slim_v_manifest_length = slim_bytes_len(slim_v_manifest_source);
(void)slim_v_manifest_length;
{
int64_t slim_v_maximum_manifest = {0};
slim_v_maximum_manifest = INT64_C(67108856);
(void)slim_v_maximum_manifest;
{
bool slim_v_manifest_fits = {0};
slim_v_manifest_fits = slim_v_manifest_length <= slim_v_maximum_manifest;
(void)slim_v_manifest_fits;
if (!slim_v_manifest_fits) {
slim_result = slim_fn_cache_invalid_key(slim_allocation_region);
}
else {
{
SlimVec slim_v_manifest_tokens = {0};
slim_v_manifest_tokens = slim_vec_new(sizeof(Slim_type_syntax_Token), slim_allocation_region);
(void)slim_v_manifest_tokens;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_lex(slim_v_manifest_source, INT64_C(0), &slim_v_manifest_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_length_bytes = {0};
slim_v_length_bytes = slim_fn_cache_append_u64(slim_v_manifest_length, INT64_C(72057594037927936), INT64_C(8), &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_length_bytes;
{
SlimUnit slim_v_manifest = {0};
slim_v_manifest = slim_fn_text_append_text(&slim_v_output, slim_v_manifest_source, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_manifest;
{
int64_t slim_v_entry_form = {0};
slim_v_entry_form = INT64_C(3);
(void)slim_v_entry_form;
{
int64_t slim_v_modules = {0};
slim_v_modules = slim_fn_syntax_skip_form(&slim_v_manifest_tokens, slim_v_entry_form, slim_allocation_region);
(void)slim_v_modules;
{
bool slim_v_valid = {0};
slim_v_valid = slim_fn_cache_append_module_key(slim_v_manifest_path, slim_v_manifest_source, &slim_v_manifest_tokens, slim_v_modules, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_valid;
{
SlimBytes slim_v_bytes = {0};
slim_v_bytes = slim_bytes_freeze(slim_v_output);
(void)slim_v_bytes;
slim_result = slim_fn_cache_make_key(slim_v_valid, slim_v_bytes, slim_allocation_region);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimBytes slim_fn_cache_encode(SlimBytes slim_v_key, SlimBytes slim_v_artifact, SlimRegion *slim_region) {
(void)slim_v_key;
(void)slim_v_artifact;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimBytes slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_key_length = {0};
slim_v_key_length = slim_bytes_len(slim_v_key);
(void)slim_v_key_length;
{
int64_t slim_v_artifact_length = {0};
slim_v_artifact_length = slim_bytes_len(slim_v_artifact);
(void)slim_v_artifact_length;
{
SlimVec slim_v_prefix = {0};
slim_v_prefix = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_prefix;
{
SlimUnit slim_v_magic = {0};
slim_v_magic = slim_fn_text_append_text(&slim_v_prefix, slim_bytes_static((const uint8_t *)"SLIMCACHE\x00", (int64_t)(sizeof("SLIMCACHE\x00") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_magic;
{
uint8_t slim_v_schema = {0};
slim_v_schema = slim_i64_to_u8(INT64_C(1));
(void)slim_v_schema;
{
SlimUnit slim_v_schema_byte = {0};
if (!slim_vec_push(&slim_v_prefix, &(slim_v_schema))) goto slim_allocation_failed; slim_v_schema_byte = (SlimUnit){0};
(void)slim_v_schema_byte;
{
SlimUnit slim_v_key_length_bytes = {0};
slim_v_key_length_bytes = slim_fn_cache_append_u64(slim_v_key_length, INT64_C(72057594037927936), INT64_C(8), &slim_v_prefix, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_key_length_bytes;
{
SlimUnit slim_v_artifact_length_bytes = {0};
slim_v_artifact_length_bytes = slim_fn_cache_append_u64(slim_v_artifact_length, INT64_C(72057594037927936), INT64_C(8), &slim_v_prefix, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_artifact_length_bytes;
{
SlimUnit slim_v_key_bytes = {0};
slim_v_key_bytes = slim_fn_text_append_text(&slim_v_prefix, slim_v_key, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_key_bytes;
{
SlimUnit slim_v_artifact_bytes = {0};
slim_v_artifact_bytes = slim_fn_text_append_text(&slim_v_prefix, slim_v_artifact, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_artifact_bytes;
{
SlimBytes slim_v_prefix_bytes = {0};
slim_v_prefix_bytes = slim_bytes_freeze(slim_v_prefix);
(void)slim_v_prefix_bytes;
{
int64_t slim_v_prefix_length = {0};
slim_v_prefix_length = slim_bytes_len(slim_v_prefix_bytes);
(void)slim_v_prefix_length;
{
int64_t slim_v_checksum = {0};
slim_v_checksum = slim_fn_cache_weighted_checksum(slim_v_prefix_bytes, INT64_C(0), slim_v_prefix_length, INT64_C(0), slim_allocation_region);
(void)slim_v_checksum;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_copied = {0};
slim_v_copied = slim_fn_text_append_text(&slim_v_output, slim_v_prefix_bytes, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_copied;
{
SlimUnit slim_v_checksum_bytes = {0};
slim_v_checksum_bytes = slim_fn_cache_append_u64(slim_v_checksum, INT64_C(72057594037927936), INT64_C(8), &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_checksum_bytes;
slim_result = slim_bytes_freeze(slim_v_output);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static Slim_type_cache_Probe slim_fn_cache_invalid_probe(SlimRegion *slim_region) {
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
Slim_type_cache_Probe slim_result = {0};
{
bool slim_t_1419 = {0};
slim_t_1419 = false;
int64_t slim_t_1423 = {0};
slim_t_1423 = INT64_C(0);
int64_t slim_t_1427 = {0};
slim_t_1427 = INT64_C(0);
slim_result = (Slim_type_cache_Probe){.slim_field_valid = slim_t_1419, .slim_field_artifact_start = slim_t_1423, .slim_field_artifact_end = slim_t_1427};
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static Slim_type_cache_Probe slim_fn_cache_probe(SlimBytes slim_v_cache, SlimBytes slim_v_key, SlimRegion *slim_region) {
(void)slim_v_cache;
(void)slim_v_key;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
Slim_type_cache_Probe slim_result = {0};
{
int64_t slim_v_length = {0};
slim_v_length = slim_bytes_len(slim_v_cache);
(void)slim_v_length;
{
bool slim_v_minimum = {0};
slim_v_minimum = slim_v_length >= INT64_C(35);
(void)slim_v_minimum;
if (!slim_v_minimum) {
slim_result = slim_fn_cache_invalid_probe(slim_allocation_region);
}
else {
{
SlimBytes slim_v_magic = {0};
slim_v_magic = slim_bytes_static((const uint8_t *)"SLIMCACHE\x00", (int64_t)(sizeof("SLIMCACHE\x00") - 1));
(void)slim_v_magic;
{
bool slim_v_magic_equal = {0};
slim_v_magic_equal = slim_fn_project_cross_span_chars_equal(slim_v_cache, INT64_C(0), slim_v_magic, INT64_C(0), INT64_C(0), INT64_C(10), slim_allocation_region);
(void)slim_v_magic_equal;
{
uint8_t slim_v_schema_byte = {0};
slim_v_schema_byte = slim_bytes_get(slim_v_cache, INT64_C(10));
(void)slim_v_schema_byte;
{
int64_t slim_v_schema_code = {0};
slim_v_schema_code = (int64_t)slim_v_schema_byte;
(void)slim_v_schema_code;
{
bool slim_v_schema_equal = {0};
slim_v_schema_equal = slim_v_schema_code == INT64_C(1);
(void)slim_v_schema_equal;
{
bool slim_v_header_valid = {0};
slim_v_header_valid = slim_v_magic_equal && slim_v_schema_equal;
(void)slim_v_header_valid;
{
Slim_type_cache_Number slim_v_key_number = {0};
slim_v_key_number = slim_fn_cache_read_bounded_u64(slim_v_cache, INT64_C(11), INT64_C(67108864), slim_allocation_region);
(void)slim_v_key_number;
{
bool slim_v_key_valid = {0};
slim_v_key_valid = slim_v_key_number.slim_field_valid;
(void)slim_v_key_valid;
{
int64_t slim_v_key_length = {0};
slim_v_key_length = slim_v_key_number.slim_field_value;
(void)slim_v_key_length;
{
Slim_type_cache_Number slim_v_artifact_number = {0};
slim_v_artifact_number = slim_fn_cache_read_bounded_u64(slim_v_cache, INT64_C(19), INT64_C(67108864), slim_allocation_region);
(void)slim_v_artifact_number;
{
bool slim_v_artifact_valid = {0};
slim_v_artifact_valid = slim_v_artifact_number.slim_field_valid;
(void)slim_v_artifact_valid;
{
bool slim_v_lengths_valid = {0};
slim_v_lengths_valid = slim_v_key_valid && slim_v_artifact_valid;
(void)slim_v_lengths_valid;
{
int64_t slim_v_artifact_length = {0};
slim_v_artifact_length = slim_v_artifact_number.slim_field_value;
(void)slim_v_artifact_length;
{
int64_t slim_v_artifact_start = {0};
slim_v_artifact_start = slim_i64_add(INT64_C(27), slim_v_key_length);
(void)slim_v_artifact_start;
{
int64_t slim_v_artifact_end = {0};
slim_v_artifact_end = slim_i64_add(slim_v_artifact_start, slim_v_artifact_length);
(void)slim_v_artifact_end;
{
int64_t slim_v_expected_length = {0};
slim_v_expected_length = slim_i64_add(slim_v_artifact_end, INT64_C(8));
(void)slim_v_expected_length;
{
bool slim_v_exact_length = {0};
slim_v_exact_length = slim_v_length == slim_v_expected_length;
(void)slim_v_exact_length;
{
int64_t slim_v_expected_key_length = {0};
slim_v_expected_key_length = slim_bytes_len(slim_v_key);
(void)slim_v_expected_key_length;
{
bool slim_v_same_key_length = {0};
slim_v_same_key_length = slim_v_key_length == slim_v_expected_key_length;
(void)slim_v_same_key_length;
{
bool slim_v_key_equal = {0};
if (slim_v_same_key_length) {
slim_v_key_equal = slim_fn_project_cross_span_chars_equal(slim_v_cache, INT64_C(27), slim_v_key, INT64_C(0), INT64_C(0), slim_v_key_length, slim_allocation_region);
}
else {
slim_v_key_equal = false;
}
(void)slim_v_key_equal;
{
int64_t slim_v_checksum = {0};
if (slim_v_exact_length) {
slim_v_checksum = slim_fn_cache_weighted_checksum(slim_v_cache, INT64_C(0), slim_v_artifact_end, INT64_C(0), slim_allocation_region);
}
else {
slim_v_checksum = INT64_C(0);
}
(void)slim_v_checksum;
{
Slim_type_cache_Number slim_v_checksum_number = {0};
if (slim_v_exact_length) {
slim_v_checksum_number = slim_fn_cache_read_bounded_u64(slim_v_cache, slim_v_artifact_end, INT64_C(2296836751160866950), slim_allocation_region);
}
else {
slim_v_checksum_number = slim_fn_cache_invalid_number(INT64_C(0), slim_allocation_region);
}
(void)slim_v_checksum_number;
{
bool slim_v_checksum_valid = {0};
slim_v_checksum_valid = slim_v_checksum_number.slim_field_valid;
(void)slim_v_checksum_valid;
{
int64_t slim_v_expected_checksum = {0};
slim_v_expected_checksum = slim_v_checksum_number.slim_field_value;
(void)slim_v_expected_checksum;
{
bool slim_v_checksum_equal = {0};
slim_v_checksum_equal = slim_v_checksum == slim_v_expected_checksum;
(void)slim_v_checksum_equal;
{
bool slim_v_first = {0};
slim_v_first = slim_v_header_valid && slim_v_lengths_valid;
(void)slim_v_first;
{
bool slim_v_second = {0};
slim_v_second = slim_v_exact_length && slim_v_key_equal;
(void)slim_v_second;
{
bool slim_v_third = {0};
slim_v_third = slim_v_checksum_valid && slim_v_checksum_equal;
(void)slim_v_third;
{
bool slim_v_prefix_valid = {0};
slim_v_prefix_valid = slim_v_first && slim_v_second;
(void)slim_v_prefix_valid;
{
bool slim_v_valid = {0};
slim_v_valid = slim_v_prefix_valid && slim_v_third;
(void)slim_v_valid;
{
bool slim_t_1822 = {0};
slim_t_1822 = slim_v_valid;
int64_t slim_t_1826 = {0};
slim_t_1826 = slim_v_artifact_start;
int64_t slim_t_1830 = {0};
slim_t_1830 = slim_v_artifact_end;
slim_result = (Slim_type_cache_Probe){.slim_field_valid = slim_t_1822, .slim_field_artifact_start = slim_t_1826, .slim_field_artifact_end = slim_t_1830};
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_cache_emit_hit(SlimBytes slim_v_cache, Slim_type_cache_Probe slim_v_cache_probe, SlimRegion *slim_region) {
(void)slim_v_cache;
(void)slim_v_cache_probe;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_marker = {0};
slim_v_marker = slim_fn_text_append_text(&slim_v_output, slim_bytes_static((const uint8_t *)"H", (int64_t)(sizeof("H") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_marker;
{
int64_t slim_v_start = {0};
slim_v_start = slim_v_cache_probe.slim_field_artifact_start;
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_v_cache_probe.slim_field_artifact_end;
(void)slim_v_end;
{
SlimUnit slim_v_artifact = {0};
slim_v_artifact = slim_fn_text_append_span(slim_v_cache, slim_v_start, slim_v_end, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_artifact;
{
SlimBytes slim_v_bytes = {0};
slim_v_bytes = slim_bytes_freeze(slim_v_output);
(void)slim_v_bytes;
{
SlimUnit slim_v_shown = {0};
slim_v_shown = slim_print_bytes(slim_v_bytes);
(void)slim_v_shown;
slim_result = INT64_C(0);
}
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_cache_emit_miss(SlimBytes slim_v_manifest_path, SlimBytes slim_v_key, SlimRegion *slim_region) {
(void)slim_v_manifest_path;
(void)slim_v_key;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_checked = {0};
slim_v_checked = slim_fn_project_check_project_path(slim_v_manifest_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_checked;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_checked > INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
slim_result = slim_v_checked;
}
else {
{
SlimBytes slim_v_artifact = {0};
slim_v_artifact = slim_fn_project_generate_project_path(slim_v_manifest_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_artifact;
{
SlimBytes slim_v_entry = {0};
slim_v_entry = slim_fn_cache_encode(slim_v_key, slim_v_artifact, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_entry;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_marker = {0};
slim_v_marker = slim_fn_text_append_text(&slim_v_output, slim_bytes_static((const uint8_t *)"M", (int64_t)(sizeof("M") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_marker;
{
SlimUnit slim_v_cache_bytes = {0};
slim_v_cache_bytes = slim_fn_text_append_text(&slim_v_output, slim_v_entry, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_cache_bytes;
{
SlimBytes slim_v_bytes = {0};
slim_v_bytes = slim_bytes_freeze(slim_v_output);
(void)slim_v_bytes;
{
SlimUnit slim_v_shown = {0};
slim_v_shown = slim_print_bytes(slim_v_bytes);
(void)slim_v_shown;
slim_result = INT64_C(0);
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_cache_run(SlimBytes slim_v_manifest_path, SlimBytes slim_v_cache_path, SlimRegion *slim_region) {
(void)slim_v_manifest_path;
(void)slim_v_cache_path;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
Slim_type_cache_Key slim_v_key_result = {0};
slim_v_key_result = slim_fn_cache_project_key(slim_v_manifest_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_key_result;
{
bool slim_v_key_valid = {0};
slim_v_key_valid = slim_v_key_result.slim_field_valid;
(void)slim_v_key_valid;
if (!slim_v_key_valid) {
slim_result = slim_fn_project_check_project_path(slim_v_manifest_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
SlimBytes slim_v_key = {0};
slim_v_key = slim_v_key_result.slim_field_bytes;
(void)slim_v_key;
{
SlimVec slim_v_cache_bytes = {0};
slim_v_cache_bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_cache_bytes;
{
bool slim_v_cache_read = {0};
slim_v_cache_read = slim_read_file(slim_v_cache_path, &slim_v_cache_bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_cache_read;
{
SlimBytes slim_v_cache = {0};
slim_v_cache = slim_bytes_freeze(slim_v_cache_bytes);
(void)slim_v_cache;
{
Slim_type_cache_Probe slim_v_cache_probe = {0};
if (slim_v_cache_read) {
slim_v_cache_probe = slim_fn_cache_probe(slim_v_cache, slim_v_key, slim_allocation_region);
}
else {
slim_v_cache_probe = slim_fn_cache_invalid_probe(slim_allocation_region);
}
(void)slim_v_cache_probe;
{
bool slim_v_hit = {0};
slim_v_hit = slim_v_cache_probe.slim_field_valid;
(void)slim_v_hit;
if (slim_v_hit) {
slim_result = slim_fn_cache_emit_hit(slim_v_cache, slim_v_cache_probe, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_result = slim_fn_cache_emit_miss(slim_v_manifest_path, slim_v_key, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_check_parameter_has_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_parameter, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_parameter;
(void)slim_v_name_start;
(void)slim_v_name_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
int64_t slim_v_parameter_name = {0};
slim_v_parameter_name = slim_fn_check_recur_parameter_name(slim_v_source, slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_parameter_name;
{
int64_t slim_v_candidate_start = {0};
slim_v_candidate_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_parameter_name, slim_allocation_region);
(void)slim_v_candidate_start;
{
int64_t slim_v_candidate_end = {0};
slim_v_candidate_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_parameter_name, slim_allocation_region);
(void)slim_v_candidate_end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_spans_equal(slim_v_source, slim_v_candidate_start, slim_v_candidate_end, slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_next;
slim_v_parameter = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_check_prior_let_has_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_v_name_start;
(void)slim_v_name_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_let_form = {0};
if (slim_v_form) {
slim_v_let_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"let", (int64_t)(sizeof("let") - 1)), slim_allocation_region);
}
else {
slim_v_let_form = false;
}
(void)slim_v_let_form;
{
int64_t slim_v_candidate = {0};
slim_v_candidate = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_candidate;
{
int64_t slim_v_candidate_start = {0};
slim_v_candidate_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_candidate, slim_allocation_region);
(void)slim_v_candidate_start;
{
int64_t slim_v_candidate_end = {0};
slim_v_candidate_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_candidate, slim_allocation_region);
(void)slim_v_candidate_end;
{
bool slim_v_same = {0};
if (slim_v_let_form) {
slim_v_same = slim_fn_syntax_spans_equal(slim_v_source, slim_v_candidate_start, slim_v_candidate_end, slim_v_name_start, slim_v_name_end, slim_allocation_region);
}
else {
slim_v_same = false;
}
(void)slim_v_same;
if (slim_v_same) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_check_match_name_is_known(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body_start, int64_t slim_v_name, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_body_start;
(void)slim_v_name;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_actual = {0};
slim_v_actual = slim_fn_check_atom_type_kind(slim_v_source, slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_actual;
{
bool slim_v_scalar = {0};
slim_v_scalar = slim_v_actual >= INT64_C(0);
(void)slim_v_scalar;
if (slim_v_scalar) {
slim_result = true;
}
else {
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_start;
{
int64_t slim_v_name_end = {0};
slim_v_name_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_end;
{
int64_t slim_v_first_parameter = {0};
slim_v_first_parameter = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_first_parameter;
{
bool slim_v_parameter = {0};
slim_v_parameter = slim_fn_check_parameter_has_name(slim_v_source, slim_v_tokens, slim_v_first_parameter, slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_parameter;
if (slim_v_parameter) {
slim_result = true;
}
else {
slim_result = slim_fn_check_prior_let_has_name(slim_v_source, slim_v_tokens, slim_v_body_start, slim_v_name, slim_v_name_start, slim_v_name_end, slim_allocation_region);
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_report_boolean_match_arms(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body_start, int64_t slim_v_expected, int64_t slim_v_cursor, bool slim_v_seen_true, bool slim_v_seen_false, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_body_start;
(void)slim_v_expected;
(void)slim_v_cursor;
(void)slim_v_seen_true;
(void)slim_v_seen_false;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
int64_t slim_v_pattern = {0};
slim_v_pattern = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_pattern;
{
bool slim_v_true_pattern = {0};
slim_v_true_pattern = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_pattern, slim_bytes_static((const uint8_t *)"true", (int64_t)(sizeof("true") - 1)), slim_allocation_region);
(void)slim_v_true_pattern;
{
bool slim_v_false_pattern = {0};
slim_v_false_pattern = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_pattern, slim_bytes_static((const uint8_t *)"false", (int64_t)(sizeof("false") - 1)), slim_allocation_region);
(void)slim_v_false_pattern;
{
bool slim_v_duplicate = {0};
if (slim_v_true_pattern) {
slim_v_duplicate = slim_v_seen_true;
}
else {
if (slim_v_false_pattern) {
slim_v_duplicate = slim_v_seen_false;
}
else {
slim_v_duplicate = false;
}
}
(void)slim_v_duplicate;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_closing = {0};
slim_v_closing = slim_i64_sub(slim_v_next, INT64_C(1));
(void)slim_v_closing;
{
int64_t slim_v_arm_start = {0};
slim_v_arm_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_arm_start;
{
int64_t slim_v_arm_end = {0};
slim_v_arm_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_closing, slim_allocation_region);
(void)slim_v_arm_end;
{
int64_t slim_v_duplicate_report = {0};
if (slim_v_duplicate) {
slim_v_duplicate_report = slim_fn_check_report_diagnostic(slim_bytes_static((const uint8_t *)"E0335", (int64_t)(sizeof("E0335") - 1)), slim_v_arm_start, slim_v_arm_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_duplicate_report = INT64_C(0);
}
(void)slim_v_duplicate_report;
{
int64_t slim_v_first = {0};
slim_v_first = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_first;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_check_last_form_in_list(slim_v_tokens, slim_v_first, slim_v_first, slim_allocation_region);
(void)slim_v_body;
{
int64_t slim_v_body_kind = {0};
slim_v_body_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_kind;
{
bool slim_v_named = {0};
slim_v_named = slim_v_body_kind == INT64_C(2);
(void)slim_v_named;
{
bool slim_v_known = {0};
if (slim_v_named) {
slim_v_known = slim_fn_check_match_name_is_known(slim_v_source, slim_v_tokens, slim_v_params, slim_v_body_start, slim_v_body, slim_allocation_region);
}
else {
slim_v_known = true;
}
(void)slim_v_known;
{
bool slim_v_unknown = {0};
slim_v_unknown = !slim_v_known;
(void)slim_v_unknown;
{
int64_t slim_v_body_token_start = {0};
slim_v_body_token_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_token_start;
{
int64_t slim_v_body_token_end = {0};
slim_v_body_token_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_token_end;
{
int64_t slim_v_unknown_report = {0};
if (slim_v_unknown) {
slim_v_unknown_report = slim_fn_check_report_diagnostic(slim_bytes_static((const uint8_t *)"E0314", (int64_t)(sizeof("E0314") - 1)), slim_v_body_token_start, slim_v_body_token_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_unknown_report = INT64_C(0);
}
(void)slim_v_unknown_report;
{
int64_t slim_v_actual = {0};
slim_v_actual = slim_fn_check_atom_type_kind(slim_v_source, slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_actual;
{
bool slim_v_expected_known = {0};
slim_v_expected_known = slim_v_expected >= INT64_C(0);
(void)slim_v_expected_known;
{
bool slim_v_actual_known = {0};
slim_v_actual_known = slim_v_actual >= INT64_C(0);
(void)slim_v_actual_known;
{
bool slim_v_both_known = {0};
slim_v_both_known = slim_v_expected_known && slim_v_actual_known;
(void)slim_v_both_known;
{
bool slim_v_same_type = {0};
slim_v_same_type = slim_v_expected == slim_v_actual;
(void)slim_v_same_type;
{
bool slim_v_different = {0};
slim_v_different = !slim_v_same_type;
(void)slim_v_different;
{
bool slim_v_mismatch = {0};
slim_v_mismatch = slim_v_both_known && slim_v_different;
(void)slim_v_mismatch;
{
int64_t slim_v_mismatch_report = {0};
if (slim_v_mismatch) {
slim_v_mismatch_report = slim_fn_check_report_diagnostic(slim_bytes_static((const uint8_t *)"E0344", (int64_t)(sizeof("E0344") - 1)), slim_v_body_token_start, slim_v_body_token_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_mismatch_report = INT64_C(0);
}
(void)slim_v_mismatch_report;
{
int64_t slim_v_first_total = {0};
slim_v_first_total = slim_i64_add(slim_v_duplicate_report, slim_v_unknown_report);
(void)slim_v_first_total;
{
int64_t slim_v_reported = {0};
slim_v_reported = slim_i64_add(slim_v_first_total, slim_v_mismatch_report);
(void)slim_v_reported;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
{
bool slim_v_next_seen_true = {0};
slim_v_next_seen_true = slim_v_seen_true || slim_v_true_pattern;
(void)slim_v_next_seen_true;
{
bool slim_v_next_seen_false = {0};
slim_v_next_seen_false = slim_v_seen_false || slim_v_false_pattern;
(void)slim_v_next_seen_false;
slim_v_cursor = slim_v_next;
slim_v_seen_true = slim_v_next_seen_true;
slim_v_seen_false = slim_v_next_seen_false;
slim_v_count = slim_v_total;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_report_boolean_match_span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body_start, int64_t slim_v_expected, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_body_start;
(void)slim_v_expected;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_match_form = {0};
if (slim_v_form) {
slim_v_match_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"match", (int64_t)(sizeof("match") - 1)), slim_allocation_region);
}
else {
slim_v_match_form = false;
}
(void)slim_v_match_form;
{
bool slim_v_incomplete = {0};
if (slim_v_match_form) {
slim_v_incomplete = slim_fn_check_boolean_match_incomplete(slim_v_source, slim_v_tokens, slim_v_cursor, slim_allocation_region);
}
else {
slim_v_incomplete = false;
}
(void)slim_v_incomplete;
{
int64_t slim_v_reported = {0};
if (slim_v_incomplete) {
{
int64_t slim_v_value = {0};
slim_v_value = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_value;
{
int64_t slim_v_arms = {0};
slim_v_arms = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_arms;
slim_v_reported = slim_fn_check_report_boolean_match_arms(slim_v_source, slim_v_tokens, slim_v_params, slim_v_body_start, slim_v_expected, slim_v_arms, false, false, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
slim_v_reported = INT64_C(0);
}
(void)slim_v_reported;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
slim_v_cursor = slim_v_next;
slim_v_count = slim_v_total;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_report_boolean_match_items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, int64_t slim_v_index, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_v_index;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_declaration_count = {0};
slim_v_declaration_count = ((*slim_v_declarations)).len;
(void)slim_v_declaration_count;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_declaration_count;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
Slim_type_ir_Declaration slim_v_declaration = {0};
slim_v_declaration = ((Slim_type_ir_Declaration *)((*slim_v_declarations)).data)[slim_vec_check_index(&((*slim_v_declarations)), slim_v_index)];
(void)slim_v_declaration;
{
int64_t slim_v_cursor = {0};
slim_v_cursor = slim_v_declaration.slim_field_form;
(void)slim_v_cursor;
{
bool slim_v_function_item = {0};
slim_v_function_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_item;
{
int64_t slim_v_reported = {0};
if (slim_v_function_item) {
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_type = {0};
slim_v_return_type = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_type;
{
int64_t slim_v_expected = {0};
slim_v_expected = slim_fn_syntax_scalar_type_kind(slim_v_source, slim_v_tokens, slim_v_return_type, slim_allocation_region);
(void)slim_v_expected;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_return_type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_effects, slim_allocation_region);
(void)slim_v_body;
{
int64_t slim_v_body_end = {0};
slim_v_body_end = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_end;
slim_v_reported = slim_fn_check_report_boolean_match_span(slim_v_source, slim_v_tokens, slim_v_params, slim_v_body, slim_v_expected, slim_v_body, slim_v_body_end, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
else {
slim_v_reported = INT64_C(0);
}
(void)slim_v_reported;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
slim_v_index = slim_v_next;
slim_v_count = slim_v_total;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_check_path_all(SlimBytes slim_v_path, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_input_bytes = {0};
slim_v_input_bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_input_bytes;
{
bool slim_v_input_read = {0};
slim_v_input_read = slim_read_file(slim_v_path, &slim_v_input_bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_input_read;
{
SlimBytes slim_v_input = {0};
slim_v_input = slim_bytes_freeze(slim_v_input_bytes);
(void)slim_v_input;
{
SlimVec slim_v_tokens = {0};
slim_v_tokens = slim_vec_new(sizeof(Slim_type_syntax_Token), slim_allocation_region);
(void)slim_v_tokens;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_lex(slim_v_input, INT64_C(0), &slim_v_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
{
int64_t slim_v_token_count = {0};
slim_v_token_count = (slim_v_tokens).len;
(void)slim_v_token_count;
{
int64_t slim_v_unterminated = {0};
slim_v_unterminated = slim_fn_check_find_unterminated(&slim_v_tokens, INT64_C(0), slim_v_token_count, INT64_C(0), INT64_C(0), slim_allocation_region);
(void)slim_v_unterminated;
{
bool slim_v_malformed = {0};
slim_v_malformed = slim_v_unterminated >= INT64_C(0);
(void)slim_v_malformed;
if (slim_v_malformed) {
{
SlimVec slim_v_declarations = {0};
slim_v_declarations = slim_vec_new(sizeof(Slim_type_ir_Declaration), slim_allocation_region);
(void)slim_v_declarations;
slim_result = slim_fn_check_check_path(slim_v_input, &slim_v_tokens, &slim_v_declarations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
else {
{
SlimVec slim_v_declarations = {0};
slim_v_declarations = slim_fn_syntax_index_declarations(slim_v_input, &slim_v_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_declarations;
{
Slim_type_memory_Plan slim_v_plan = {0};
slim_v_plan = slim_fn_memory_analyze(slim_v_input, &slim_v_tokens, &slim_v_declarations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_plan;
{
int64_t slim_v_checked = {0};
slim_v_checked = slim_fn_check_check_path_moves(slim_v_input, &slim_v_tokens, &slim_v_declarations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_checked;
{
bool slim_v_prior_error = {0};
slim_v_prior_error = slim_v_checked > INT64_C(0);
(void)slim_v_prior_error;
if (!slim_v_prior_error) {
slim_result = INT64_C(0);
}
else {
{
int64_t slim_v_recovered = {0};
slim_v_recovered = slim_fn_check_report_boolean_match_items(slim_v_input, &slim_v_tokens, &slim_v_declarations, INT64_C(0), INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_recovered;
slim_result = slim_v_checked;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_check_recur_parameter_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_parameter, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_parameter;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_check_parameter_is_inout(slim_v_source, slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_borrowed;
if (slim_v_borrowed) {
slim_result = slim_i64_add(slim_v_parameter, INT64_C(2));
}
else {
slim_result = slim_i64_add(slim_v_parameter, INT64_C(1));
}
}
return slim_result;
}

static int64_t slim_fn_check_report_recur_arguments(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_argument, int64_t slim_v_parameter, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_argument;
(void)slim_v_parameter;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_check_parameter_is_inout(slim_v_source, slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_borrowed;
{
int64_t slim_v_parameter_name = {0};
slim_v_parameter_name = slim_fn_check_recur_parameter_name(slim_v_source, slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_parameter_name;
{
int64_t slim_v_parameter_start = {0};
slim_v_parameter_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_parameter_name, slim_allocation_region);
(void)slim_v_parameter_start;
{
int64_t slim_v_parameter_end = {0};
slim_v_parameter_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_parameter_name, slim_allocation_region);
(void)slim_v_parameter_end;
{
int64_t slim_v_argument_start = {0};
slim_v_argument_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_argument_start;
{
int64_t slim_v_argument_end = {0};
slim_v_argument_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_argument_end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_spans_equal(slim_v_source, slim_v_parameter_start, slim_v_parameter_end, slim_v_argument_start, slim_v_argument_end, slim_allocation_region);
(void)slim_v_same;
{
bool slim_v_different = {0};
slim_v_different = !slim_v_same;
(void)slim_v_different;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_borrowed && slim_v_different;
(void)slim_v_invalid;
{
int64_t slim_v_reported = {0};
if (slim_v_invalid) {
slim_v_reported = slim_fn_check_report_diagnostic(slim_bytes_static((const uint8_t *)"E0350", (int64_t)(sizeof("E0350") - 1)), slim_v_argument_start, slim_v_argument_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_reported = INT64_C(0);
}
(void)slim_v_reported;
{
int64_t slim_v_next_argument = {0};
slim_v_next_argument = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_next_argument;
{
int64_t slim_v_next_parameter = {0};
slim_v_next_parameter = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_next_parameter;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
slim_v_argument = slim_v_next_argument;
slim_v_parameter = slim_v_next_parameter;
slim_v_count = slim_v_total;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_report_recur_span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_recur_form = {0};
if (slim_v_form) {
slim_v_recur_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"recur", (int64_t)(sizeof("recur") - 1)), slim_allocation_region);
}
else {
slim_v_recur_form = false;
}
(void)slim_v_recur_form;
{
int64_t slim_v_reported = {0};
if (slim_v_recur_form) {
{
int64_t slim_v_argument = {0};
slim_v_argument = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_argument;
{
int64_t slim_v_parameter = {0};
slim_v_parameter = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_parameter;
slim_v_reported = slim_fn_check_report_recur_arguments(slim_v_source, slim_v_tokens, slim_v_argument, slim_v_parameter, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
slim_v_reported = INT64_C(0);
}
(void)slim_v_reported;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
slim_v_cursor = slim_v_next;
slim_v_count = slim_v_total;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_report_recur_items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, int64_t slim_v_index, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_v_index;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_declaration_count = {0};
slim_v_declaration_count = ((*slim_v_declarations)).len;
(void)slim_v_declaration_count;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_declaration_count;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
Slim_type_ir_Declaration slim_v_declaration = {0};
slim_v_declaration = ((Slim_type_ir_Declaration *)((*slim_v_declarations)).data)[slim_vec_check_index(&((*slim_v_declarations)), slim_v_index)];
(void)slim_v_declaration;
{
int64_t slim_v_cursor = {0};
slim_v_cursor = slim_v_declaration.slim_field_form;
(void)slim_v_cursor;
{
bool slim_v_function_item = {0};
slim_v_function_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_item;
{
int64_t slim_v_reported = {0};
if (slim_v_function_item) {
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_type = {0};
slim_v_return_type = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_type;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_return_type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_effects, slim_allocation_region);
(void)slim_v_body;
{
int64_t slim_v_body_end = {0};
slim_v_body_end = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_end;
slim_v_reported = slim_fn_check_report_recur_span(slim_v_source, slim_v_tokens, slim_v_params, slim_v_body, slim_v_body_end, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
else {
slim_v_reported = INT64_C(0);
}
(void)slim_v_reported;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
slim_v_index = slim_v_next;
slim_v_count = slim_v_total;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_check_path_recur(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_checked = {0};
slim_v_checked = slim_fn_check_check_path_inout(slim_v_input, slim_v_tokens, slim_v_declarations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_checked;
{
bool slim_v_prior_error = {0};
slim_v_prior_error = slim_v_checked > INT64_C(0);
(void)slim_v_prior_error;
if (slim_v_prior_error) {
slim_result = slim_v_checked;
}
else {
{
int64_t slim_v_errors = {0};
slim_v_errors = slim_fn_check_report_recur_items(slim_v_input, slim_v_tokens, slim_v_declarations, INT64_C(0), INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_errors;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_errors > INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
slim_result = INT64_C(1);
}
else {
slim_result = INT64_C(0);
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_check_ownership_type_is_copy(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_type_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_type_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_storage = {0};
slim_v_storage = slim_fn_memory_type_storage_kind(slim_v_source, slim_v_tokens, slim_v_type_index, slim_allocation_region);
(void)slim_v_storage;
slim_result = slim_v_storage < INT64_C(2);
}
return slim_result;
}

static int64_t slim_fn_check_ownership_parameter_type(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_parameter, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_parameter;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_check_parameter_is_inout(slim_v_source, slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_borrowed;
if (slim_v_borrowed) {
slim_result = slim_i64_add(slim_v_parameter, INT64_C(3));
}
else {
slim_result = slim_i64_add(slim_v_parameter, INT64_C(2));
}
}
return slim_result;
}

static bool slim_fn_check_moved_has(SlimBytes slim_v_source, SlimVec * slim_v_moved, int64_t slim_v_index, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_moved;
(void)slim_v_index;
(void)slim_v_name_start;
(void)slim_v_name_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_moved)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
Slim_type_syntax_Token slim_v_candidate = {0};
slim_v_candidate = ((Slim_type_syntax_Token *)((*slim_v_moved)).data)[slim_vec_check_index(&((*slim_v_moved)), slim_v_index)];
(void)slim_v_candidate;
{
int64_t slim_v_candidate_start = {0};
slim_v_candidate_start = slim_v_candidate.slim_field_start;
(void)slim_v_candidate_start;
{
int64_t slim_v_candidate_end = {0};
slim_v_candidate_end = slim_v_candidate.slim_field_end;
(void)slim_v_candidate_end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_spans_equal(slim_v_source, slim_v_candidate_start, slim_v_candidate_end, slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_report_owned_arguments(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_current_params, int64_t slim_v_argument, int64_t slim_v_parameter, SlimVec * slim_v_moved, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_current_params;
(void)slim_v_argument;
(void)slim_v_parameter;
(void)slim_v_moved;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_check_parameter_is_inout(slim_v_source, slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_borrowed;
{
int64_t slim_v_type_index = {0};
slim_v_type_index = slim_fn_check_ownership_parameter_type(slim_v_source, slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_type_index;
{
bool slim_v_copy = {0};
slim_v_copy = slim_fn_check_ownership_type_is_copy(slim_v_source, slim_v_tokens, slim_v_type_index, slim_allocation_region);
(void)slim_v_copy;
{
int64_t slim_v_argument_kind = {0};
slim_v_argument_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_argument_kind;
{
bool slim_v_named = {0};
slim_v_named = slim_v_argument_kind == INT64_C(2);
(void)slim_v_named;
{
bool slim_v_owned = {0};
slim_v_owned = !slim_v_borrowed;
(void)slim_v_owned;
{
bool slim_v_noncopy = {0};
slim_v_noncopy = !slim_v_copy;
(void)slim_v_noncopy;
{
bool slim_v_first = {0};
slim_v_first = slim_v_owned && slim_v_noncopy;
(void)slim_v_first;
{
bool slim_v_moving_name = {0};
slim_v_moving_name = slim_v_first && slim_v_named;
(void)slim_v_moving_name;
{
int64_t slim_v_reported = {0};
if (slim_v_moving_name) {
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_name_start;
{
int64_t slim_v_name_end = {0};
slim_v_name_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_name_end;
{
bool slim_v_already_moved = {0};
slim_v_already_moved = slim_fn_check_moved_has(slim_v_source, slim_v_moved, INT64_C(0), slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_already_moved;
if (slim_v_already_moved) {
slim_v_reported = slim_fn_check_report_diagnostic(slim_bytes_static((const uint8_t *)"E0315", (int64_t)(sizeof("E0315") - 1)), slim_v_name_start, slim_v_name_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_from_inout = {0};
slim_v_from_inout = slim_fn_syntax_name_is_inout(slim_v_source, slim_v_tokens, slim_v_current_params, slim_v_argument, slim_allocation_region);
(void)slim_v_from_inout;
if (slim_v_from_inout) {
slim_v_reported = slim_fn_check_report_diagnostic(slim_bytes_static((const uint8_t *)"E0347", (int64_t)(sizeof("E0347") - 1)), slim_v_name_start, slim_v_name_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
Slim_type_syntax_Token slim_v_token = {0};
slim_v_token = ((Slim_type_syntax_Token *)((*slim_v_tokens)).data)[slim_vec_check_index(&((*slim_v_tokens)), slim_v_argument)];
(void)slim_v_token;
{
SlimUnit slim_v_added = {0};
if (!slim_vec_push(slim_v_moved, &(slim_v_token))) goto slim_allocation_failed; slim_v_added = (SlimUnit){0};
(void)slim_v_added;
slim_v_reported = INT64_C(0);
}
}
}
}
}
}
}
}
}
else {
slim_v_reported = INT64_C(0);
}
(void)slim_v_reported;
{
int64_t slim_v_next_argument = {0};
slim_v_next_argument = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_next_argument;
{
int64_t slim_v_next_parameter = {0};
slim_v_next_parameter = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_next_parameter;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
slim_v_argument = slim_v_next_argument;
slim_v_parameter = slim_v_next_parameter;
slim_v_count = slim_v_total;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_report_builtin_move(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_current_params, int64_t slim_v_callee, int64_t slim_v_argument, SlimVec * slim_v_moved, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_current_params;
(void)slim_v_callee;
(void)slim_v_argument;
(void)slim_v_moved;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_freezing = {0};
slim_v_freezing = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bytes.freeze", (int64_t)(sizeof("bytes.freeze") - 1)), slim_allocation_region);
(void)slim_v_freezing;
if (!slim_v_freezing) {
slim_result = INT64_C(0);
}
else {
{
int64_t slim_v_argument_kind = {0};
slim_v_argument_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_argument_kind;
{
bool slim_v_named = {0};
slim_v_named = slim_v_argument_kind == INT64_C(2);
(void)slim_v_named;
if (!slim_v_named) {
slim_result = INT64_C(0);
}
else {
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_name_start;
{
int64_t slim_v_name_end = {0};
slim_v_name_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_name_end;
{
bool slim_v_already_moved = {0};
slim_v_already_moved = slim_fn_check_moved_has(slim_v_source, slim_v_moved, INT64_C(0), slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_already_moved;
if (slim_v_already_moved) {
slim_result = slim_fn_check_report_diagnostic(slim_bytes_static((const uint8_t *)"E0315", (int64_t)(sizeof("E0315") - 1)), slim_v_name_start, slim_v_name_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_from_inout = {0};
slim_v_from_inout = slim_fn_syntax_name_is_inout(slim_v_source, slim_v_tokens, slim_v_current_params, slim_v_argument, slim_allocation_region);
(void)slim_v_from_inout;
if (slim_v_from_inout) {
slim_result = slim_fn_check_report_diagnostic(slim_bytes_static((const uint8_t *)"E0347", (int64_t)(sizeof("E0347") - 1)), slim_v_name_start, slim_v_name_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
Slim_type_syntax_Token slim_v_token = {0};
slim_v_token = ((Slim_type_syntax_Token *)((*slim_v_tokens)).data)[slim_vec_check_index(&((*slim_v_tokens)), slim_v_argument)];
(void)slim_v_token;
{
SlimUnit slim_v_added = {0};
if (!slim_vec_push(slim_v_moved, &(slim_v_token))) goto slim_allocation_failed; slim_v_added = (SlimUnit){0};
(void)slim_v_added;
slim_result = INT64_C(0);
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_report_moves_span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_current_params, int64_t slim_v_cursor, int64_t slim_v_end, SlimVec * slim_v_moved, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_current_params;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_v_moved;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_call_form = {0};
if (slim_v_form) {
slim_v_call_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"call", (int64_t)(sizeof("call") - 1)), slim_allocation_region);
}
else {
slim_v_call_form = false;
}
(void)slim_v_call_form;
{
int64_t slim_v_reported = {0};
if (slim_v_call_form) {
{
int64_t slim_v_callee = {0};
slim_v_callee = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_callee;
{
int64_t slim_v_argument = {0};
slim_v_argument = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_argument;
{
int64_t slim_v_callee_start = {0};
slim_v_callee_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_callee_start;
{
int64_t slim_v_callee_end = {0};
slim_v_callee_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_callee_end;
{
int64_t slim_v_callee_params = {0};
slim_v_callee_params = slim_fn_syntax_find_function_params(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_callee_start, slim_v_callee_end, slim_allocation_region);
(void)slim_v_callee_params;
{
bool slim_v_found = {0};
slim_v_found = slim_v_callee_params >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
{
int64_t slim_v_parameter = {0};
slim_v_parameter = slim_i64_add(slim_v_callee_params, INT64_C(1));
(void)slim_v_parameter;
slim_v_reported = slim_fn_check_report_owned_arguments(slim_v_source, slim_v_tokens, slim_v_current_params, slim_v_argument, slim_v_parameter, slim_v_moved, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
else {
slim_v_reported = slim_fn_check_report_builtin_move(slim_v_source, slim_v_tokens, slim_v_current_params, slim_v_callee, slim_v_argument, slim_v_moved, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
else {
slim_v_reported = INT64_C(0);
}
(void)slim_v_reported;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
slim_v_cursor = slim_v_next;
slim_v_count = slim_v_total;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_report_inout_return(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_return_type, int64_t slim_v_body, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_return_type;
(void)slim_v_body;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_copy = {0};
slim_v_copy = slim_fn_check_ownership_type_is_copy(slim_v_source, slim_v_tokens, slim_v_return_type, slim_allocation_region);
(void)slim_v_copy;
if (slim_v_copy) {
slim_result = INT64_C(0);
}
else {
{
int64_t slim_v_result = {0};
slim_v_result = slim_fn_check_result_expr(slim_v_source, slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_result;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_result, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_named = {0};
slim_v_named = slim_v_kind == INT64_C(2);
(void)slim_v_named;
if (!slim_v_named) {
slim_result = INT64_C(0);
}
else {
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_name_is_inout(slim_v_source, slim_v_tokens, slim_v_params, slim_v_result, slim_allocation_region);
(void)slim_v_borrowed;
if (!slim_v_borrowed) {
slim_result = INT64_C(0);
}
else {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_result, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_result, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_check_report_diagnostic(slim_bytes_static((const uint8_t *)"E0347", (int64_t)(sizeof("E0347") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_report_move_items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, SlimVec * slim_v_declarations, int64_t slim_v_index, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_declarations;
(void)slim_v_index;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_declaration_count = {0};
slim_v_declaration_count = ((*slim_v_declarations)).len;
(void)slim_v_declaration_count;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_declaration_count;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
Slim_type_ir_Declaration slim_v_declaration = {0};
slim_v_declaration = ((Slim_type_ir_Declaration *)((*slim_v_declarations)).data)[slim_vec_check_index(&((*slim_v_declarations)), slim_v_index)];
(void)slim_v_declaration;
{
int64_t slim_v_cursor = {0};
slim_v_cursor = slim_v_declaration.slim_field_form;
(void)slim_v_cursor;
{
bool slim_v_function_item = {0};
slim_v_function_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_item;
{
int64_t slim_v_reported = {0};
if (slim_v_function_item) {
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_type = {0};
slim_v_return_type = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_type;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_return_type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_effects, slim_allocation_region);
(void)slim_v_body;
{
int64_t slim_v_body_end = {0};
slim_v_body_end = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_end;
{
SlimVec slim_v_moved = {0};
slim_v_moved = slim_vec_new(sizeof(Slim_type_syntax_Token), slim_allocation_region);
(void)slim_v_moved;
{
int64_t slim_v_returns = {0};
slim_v_returns = slim_fn_check_report_inout_return(slim_v_source, slim_v_tokens, slim_v_params, slim_v_return_type, slim_v_body, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_returns;
{
int64_t slim_v_body_moves = {0};
slim_v_body_moves = slim_fn_check_report_moves_span(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_params, slim_v_body, slim_v_body_end, &slim_v_moved, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_body_moves;
slim_v_reported = slim_i64_add(slim_v_returns, slim_v_body_moves);
}
}
}
}
}
}
}
}
}
else {
slim_v_reported = INT64_C(0);
}
(void)slim_v_reported;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
slim_v_index = slim_v_next;
slim_v_count = slim_v_total;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_check_path_moves(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_checked = {0};
slim_v_checked = slim_fn_check_check_path_recur(slim_v_input, slim_v_tokens, slim_v_declarations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_checked;
{
bool slim_v_prior_error = {0};
slim_v_prior_error = slim_v_checked > INT64_C(0);
(void)slim_v_prior_error;
if (slim_v_prior_error) {
slim_result = slim_v_checked;
}
else {
{
int64_t slim_v_errors = {0};
slim_v_errors = slim_fn_check_report_move_items(slim_v_input, slim_v_tokens, INT64_C(3), slim_v_declarations, INT64_C(0), INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_errors;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_errors > INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
slim_result = INT64_C(1);
}
else {
slim_result = INT64_C(0);
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_check_parameter_is_inout(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_parameter, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_parameter;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_parameter, INT64_C(1));
(void)slim_v_head;
slim_result = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"inout", (int64_t)(sizeof("inout") - 1)), slim_allocation_region);
}
return slim_result;
}

static int64_t slim_fn_check_find_inout_temporary_args(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_argument, int64_t slim_v_parameter, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_argument;
(void)slim_v_parameter;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_check_parameter_is_inout(slim_v_source, slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_borrowed;
{
int64_t slim_v_argument_kind = {0};
slim_v_argument_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_argument_kind;
{
bool slim_v_named = {0};
slim_v_named = slim_v_argument_kind == INT64_C(2);
(void)slim_v_named;
{
bool slim_v_not_named = {0};
slim_v_not_named = !slim_v_named;
(void)slim_v_not_named;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_borrowed && slim_v_not_named;
(void)slim_v_invalid;
if (slim_v_invalid) {
slim_result = slim_v_argument;
}
else {
{
int64_t slim_v_next_argument = {0};
slim_v_next_argument = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_next_argument;
{
int64_t slim_v_next_parameter = {0};
slim_v_next_parameter = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_next_parameter;
slim_v_argument = slim_v_next_argument;
slim_v_parameter = slim_v_next_parameter;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_find_inout_temporary_span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_cursor, int64_t slim_v_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_call_form = {0};
if (slim_v_form) {
slim_v_call_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"call", (int64_t)(sizeof("call") - 1)), slim_allocation_region);
}
else {
slim_v_call_form = false;
}
(void)slim_v_call_form;
{
int64_t slim_v_invalid = {0};
if (slim_v_call_form) {
{
int64_t slim_v_callee = {0};
slim_v_callee = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_callee;
{
int64_t slim_v_callee_start = {0};
slim_v_callee_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_callee_start;
{
int64_t slim_v_callee_end = {0};
slim_v_callee_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_callee_end;
{
int64_t slim_v_params = {0};
slim_v_params = slim_fn_syntax_find_function_params(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_callee_start, slim_v_callee_end, slim_allocation_region);
(void)slim_v_params;
{
bool slim_v_found = {0};
slim_v_found = slim_v_params >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
{
int64_t slim_v_argument = {0};
slim_v_argument = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_argument;
{
int64_t slim_v_parameter = {0};
slim_v_parameter = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_parameter;
slim_v_invalid = slim_fn_check_find_inout_temporary_args(slim_v_source, slim_v_tokens, slim_v_argument, slim_v_parameter, slim_allocation_region);
}
}
}
else {
slim_v_invalid = INT64_C(-1);
}
}
}
}
}
}
}
else {
slim_v_invalid = INT64_C(-1);
}
(void)slim_v_invalid;
{
bool slim_v_found = {0};
slim_v_found = slim_v_invalid >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_invalid;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_find_inout_temporary_items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_declarations;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_declaration_count = {0};
slim_v_declaration_count = ((*slim_v_declarations)).len;
(void)slim_v_declaration_count;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_declaration_count;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
Slim_type_ir_Declaration slim_v_declaration = {0};
slim_v_declaration = ((Slim_type_ir_Declaration *)((*slim_v_declarations)).data)[slim_vec_check_index(&((*slim_v_declarations)), slim_v_index)];
(void)slim_v_declaration;
{
int64_t slim_v_cursor = {0};
slim_v_cursor = slim_v_declaration.slim_field_form;
(void)slim_v_cursor;
{
bool slim_v_function_item = {0};
slim_v_function_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_item;
{
int64_t slim_v_invalid = {0};
if (slim_v_function_item) {
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_type = {0};
slim_v_return_type = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_type;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_return_type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_effects, slim_allocation_region);
(void)slim_v_body;
{
int64_t slim_v_body_end = {0};
slim_v_body_end = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_end;
slim_v_invalid = slim_fn_check_find_inout_temporary_span(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_body, slim_v_body_end, slim_allocation_region);
}
}
}
}
}
}
else {
slim_v_invalid = INT64_C(-1);
}
(void)slim_v_invalid;
{
bool slim_v_found = {0};
slim_v_found = slim_v_invalid >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_invalid;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_check_prior_inout_alias(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_argument, int64_t slim_v_parameter, int64_t slim_v_current_argument, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_argument;
(void)slim_v_parameter;
(void)slim_v_current_argument;
(void)slim_v_name_start;
(void)slim_v_name_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
bool slim_v_reached = {0};
slim_v_reached = slim_v_argument == slim_v_current_argument;
(void)slim_v_reached;
if (slim_v_reached) {
slim_result = false;
}
else {
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_check_parameter_is_inout(slim_v_source, slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_borrowed;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_named = {0};
slim_v_named = slim_v_kind == INT64_C(2);
(void)slim_v_named;
{
bool slim_v_comparable = {0};
slim_v_comparable = slim_v_borrowed && slim_v_named;
(void)slim_v_comparable;
{
int64_t slim_v_candidate_start = {0};
slim_v_candidate_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_candidate_start;
{
int64_t slim_v_candidate_end = {0};
slim_v_candidate_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_candidate_end;
{
bool slim_v_same = {0};
if (slim_v_comparable) {
slim_v_same = slim_fn_syntax_spans_equal(slim_v_source, slim_v_candidate_start, slim_v_candidate_end, slim_v_name_start, slim_v_name_end, slim_allocation_region);
}
else {
slim_v_same = false;
}
(void)slim_v_same;
if (slim_v_same) {
slim_result = true;
}
else {
{
int64_t slim_v_next_argument = {0};
slim_v_next_argument = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_next_argument;
{
int64_t slim_v_next_parameter = {0};
slim_v_next_parameter = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_next_parameter;
slim_v_argument = slim_v_next_argument;
slim_v_parameter = slim_v_next_parameter;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_find_inout_alias_args(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_first_argument, int64_t slim_v_first_parameter, int64_t slim_v_argument, int64_t slim_v_parameter, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_first_argument;
(void)slim_v_first_parameter;
(void)slim_v_argument;
(void)slim_v_parameter;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_check_parameter_is_inout(slim_v_source, slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_borrowed;
{
int64_t slim_v_argument_kind = {0};
slim_v_argument_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_argument_kind;
{
bool slim_v_named = {0};
slim_v_named = slim_v_argument_kind == INT64_C(2);
(void)slim_v_named;
{
bool slim_v_comparable = {0};
slim_v_comparable = slim_v_borrowed && slim_v_named;
(void)slim_v_comparable;
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_name_start;
{
int64_t slim_v_name_end = {0};
slim_v_name_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_name_end;
{
bool slim_v_duplicate = {0};
if (slim_v_comparable) {
slim_v_duplicate = slim_fn_check_prior_inout_alias(slim_v_source, slim_v_tokens, slim_v_first_argument, slim_v_first_parameter, slim_v_argument, slim_v_name_start, slim_v_name_end, slim_allocation_region);
}
else {
slim_v_duplicate = false;
}
(void)slim_v_duplicate;
if (slim_v_duplicate) {
slim_result = slim_v_argument;
}
else {
{
int64_t slim_v_next_argument = {0};
slim_v_next_argument = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_next_argument;
{
int64_t slim_v_next_parameter = {0};
slim_v_next_parameter = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_next_parameter;
slim_v_argument = slim_v_next_argument;
slim_v_parameter = slim_v_next_parameter;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_find_inout_alias_span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_cursor, int64_t slim_v_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_call_form = {0};
if (slim_v_form) {
slim_v_call_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"call", (int64_t)(sizeof("call") - 1)), slim_allocation_region);
}
else {
slim_v_call_form = false;
}
(void)slim_v_call_form;
{
int64_t slim_v_invalid = {0};
if (slim_v_call_form) {
{
int64_t slim_v_callee = {0};
slim_v_callee = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_callee;
{
int64_t slim_v_callee_start = {0};
slim_v_callee_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_callee_start;
{
int64_t slim_v_callee_end = {0};
slim_v_callee_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_callee_end;
{
int64_t slim_v_params = {0};
slim_v_params = slim_fn_syntax_find_function_params(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_callee_start, slim_v_callee_end, slim_allocation_region);
(void)slim_v_params;
{
bool slim_v_found = {0};
slim_v_found = slim_v_params >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
{
int64_t slim_v_argument = {0};
slim_v_argument = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_argument;
{
int64_t slim_v_parameter = {0};
slim_v_parameter = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_parameter;
slim_v_invalid = slim_fn_check_find_inout_alias_args(slim_v_source, slim_v_tokens, slim_v_argument, slim_v_parameter, slim_v_argument, slim_v_parameter, slim_allocation_region);
}
}
}
else {
slim_v_invalid = INT64_C(-1);
}
}
}
}
}
}
}
else {
slim_v_invalid = INT64_C(-1);
}
(void)slim_v_invalid;
{
bool slim_v_found = {0};
slim_v_found = slim_v_invalid >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_invalid;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_find_inout_alias_items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_declarations;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_declaration_count = {0};
slim_v_declaration_count = ((*slim_v_declarations)).len;
(void)slim_v_declaration_count;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_declaration_count;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
Slim_type_ir_Declaration slim_v_declaration = {0};
slim_v_declaration = ((Slim_type_ir_Declaration *)((*slim_v_declarations)).data)[slim_vec_check_index(&((*slim_v_declarations)), slim_v_index)];
(void)slim_v_declaration;
{
int64_t slim_v_cursor = {0};
slim_v_cursor = slim_v_declaration.slim_field_form;
(void)slim_v_cursor;
{
bool slim_v_function_item = {0};
slim_v_function_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_item;
{
int64_t slim_v_invalid = {0};
if (slim_v_function_item) {
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_type = {0};
slim_v_return_type = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_type;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_return_type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_effects, slim_allocation_region);
(void)slim_v_body;
{
int64_t slim_v_body_end = {0};
slim_v_body_end = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_end;
slim_v_invalid = slim_fn_check_find_inout_alias_span(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_body, slim_v_body_end, slim_allocation_region);
}
}
}
}
}
}
else {
slim_v_invalid = INT64_C(-1);
}
(void)slim_v_invalid;
{
bool slim_v_found = {0};
slim_v_found = slim_v_invalid >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_invalid;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_check_path_inout(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_exhaustive = {0};
slim_v_exhaustive = slim_fn_check_check_path_exhaustive(slim_v_input, slim_v_tokens, slim_v_declarations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_exhaustive;
{
bool slim_v_prior_error = {0};
slim_v_prior_error = slim_v_exhaustive > INT64_C(0);
(void)slim_v_prior_error;
if (slim_v_prior_error) {
slim_result = slim_v_exhaustive;
}
else {
{
int64_t slim_v_temporary = {0};
slim_v_temporary = slim_fn_check_find_inout_temporary_items(slim_v_input, slim_v_tokens, INT64_C(3), slim_v_declarations, INT64_C(0), slim_allocation_region);
(void)slim_v_temporary;
{
bool slim_v_has_temporary = {0};
slim_v_has_temporary = slim_v_temporary >= INT64_C(0);
(void)slim_v_has_temporary;
if (slim_v_has_temporary) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_temporary, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_temporary, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_closing = {0};
slim_v_closing = slim_i64_sub(slim_v_next, INT64_C(1));
(void)slim_v_closing;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_closing, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_check_report_diagnostic(slim_bytes_static((const uint8_t *)"E0348", (int64_t)(sizeof("E0348") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
else {
{
int64_t slim_v_duplicate = {0};
slim_v_duplicate = slim_fn_check_find_inout_alias_items(slim_v_input, slim_v_tokens, INT64_C(3), slim_v_declarations, INT64_C(0), slim_allocation_region);
(void)slim_v_duplicate;
{
bool slim_v_has_duplicate = {0};
slim_v_has_duplicate = slim_v_duplicate >= INT64_C(0);
(void)slim_v_has_duplicate;
if (slim_v_has_duplicate) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_duplicate, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_duplicate, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_check_report_diagnostic(slim_bytes_static((const uint8_t *)"E0349", (int64_t)(sizeof("E0349") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
slim_result = INT64_C(0);
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_check_boolean_arms_have(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimBytes slim_v_pattern, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_pattern;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
int64_t slim_v_arm_pattern = {0};
slim_v_arm_pattern = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_arm_pattern;
{
bool slim_v_found = {0};
slim_v_found = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_arm_pattern, slim_v_pattern, slim_allocation_region);
(void)slim_v_found;
if (slim_v_found) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_check_boolean_match_incomplete(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_expr;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_value = {0};
slim_v_value = slim_i64_add(slim_v_expr, INT64_C(2));
(void)slim_v_value;
{
bool slim_v_true_value = {0};
slim_v_true_value = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_value, slim_bytes_static((const uint8_t *)"true", (int64_t)(sizeof("true") - 1)), slim_allocation_region);
(void)slim_v_true_value;
{
bool slim_v_false_value = {0};
slim_v_false_value = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_value, slim_bytes_static((const uint8_t *)"false", (int64_t)(sizeof("false") - 1)), slim_allocation_region);
(void)slim_v_false_value;
{
bool slim_v_boolean_value = {0};
slim_v_boolean_value = slim_v_true_value || slim_v_false_value;
(void)slim_v_boolean_value;
if (!slim_v_boolean_value) {
slim_result = false;
}
else {
{
int64_t slim_v_arms = {0};
slim_v_arms = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_arms;
{
bool slim_v_has_true = {0};
slim_v_has_true = slim_fn_check_boolean_arms_have(slim_v_source, slim_v_tokens, slim_v_arms, slim_bytes_static((const uint8_t *)"true", (int64_t)(sizeof("true") - 1)), slim_allocation_region);
(void)slim_v_has_true;
{
bool slim_v_has_false = {0};
slim_v_has_false = slim_fn_check_boolean_arms_have(slim_v_source, slim_v_tokens, slim_v_arms, slim_bytes_static((const uint8_t *)"false", (int64_t)(sizeof("false") - 1)), slim_allocation_region);
(void)slim_v_has_false;
{
bool slim_v_complete = {0};
slim_v_complete = slim_v_has_true && slim_v_has_false;
(void)slim_v_complete;
slim_result = !slim_v_complete;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_find_nonexhaustive_in_span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_match_form = {0};
if (slim_v_form) {
slim_v_match_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"match", (int64_t)(sizeof("match") - 1)), slim_allocation_region);
}
else {
slim_v_match_form = false;
}
(void)slim_v_match_form;
{
bool slim_v_incomplete = {0};
if (slim_v_match_form) {
slim_v_incomplete = slim_fn_check_boolean_match_incomplete(slim_v_source, slim_v_tokens, slim_v_cursor, slim_allocation_region);
}
else {
slim_v_incomplete = false;
}
(void)slim_v_incomplete;
if (slim_v_incomplete) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_find_nonexhaustive_items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_declarations;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_declaration_count = {0};
slim_v_declaration_count = ((*slim_v_declarations)).len;
(void)slim_v_declaration_count;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_declaration_count;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
Slim_type_ir_Declaration slim_v_declaration = {0};
slim_v_declaration = ((Slim_type_ir_Declaration *)((*slim_v_declarations)).data)[slim_vec_check_index(&((*slim_v_declarations)), slim_v_index)];
(void)slim_v_declaration;
{
int64_t slim_v_cursor = {0};
slim_v_cursor = slim_v_declaration.slim_field_form;
(void)slim_v_cursor;
{
bool slim_v_function_item = {0};
slim_v_function_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_item;
{
int64_t slim_v_incomplete = {0};
if (slim_v_function_item) {
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_type = {0};
slim_v_return_type = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_type;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_return_type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_effects, slim_allocation_region);
(void)slim_v_body;
{
int64_t slim_v_body_end = {0};
slim_v_body_end = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_end;
slim_v_incomplete = slim_fn_check_find_nonexhaustive_in_span(slim_v_source, slim_v_tokens, slim_v_body, slim_v_body_end, slim_allocation_region);
}
}
}
}
}
}
else {
slim_v_incomplete = INT64_C(-1);
}
(void)slim_v_incomplete;
{
bool slim_v_found = {0};
slim_v_found = slim_v_incomplete >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_incomplete;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_check_path_exhaustive(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_checked = {0};
slim_v_checked = slim_fn_check_check_path_effects(slim_v_input, slim_v_tokens, slim_v_declarations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_checked;
{
bool slim_v_prior_error = {0};
slim_v_prior_error = slim_v_checked > INT64_C(0);
(void)slim_v_prior_error;
if (slim_v_prior_error) {
slim_result = slim_v_checked;
}
else {
{
int64_t slim_v_incomplete = {0};
slim_v_incomplete = slim_fn_check_find_nonexhaustive_items(slim_v_input, slim_v_tokens, INT64_C(3), slim_v_declarations, INT64_C(0), slim_allocation_region);
(void)slim_v_incomplete;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_incomplete >= INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_incomplete, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_incomplete, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_closing = {0};
slim_v_closing = slim_i64_sub(slim_v_next, INT64_C(1));
(void)slim_v_closing;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_closing, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_check_report_diagnostic(slim_bytes_static((const uint8_t *)"E0336", (int64_t)(sizeof("E0336") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
else {
slim_result = INT64_C(0);
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_check_effect_list_has(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_effect_code, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_effect_code;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
bool slim_v_alloc_effect = {0};
slim_v_alloc_effect = slim_v_effect_code == INT64_C(1);
(void)slim_v_alloc_effect;
{
bool slim_v_io_effect = {0};
slim_v_io_effect = slim_v_effect_code == INT64_C(2);
(void)slim_v_io_effect;
{
bool slim_v_matches_alloc = {0};
slim_v_matches_alloc = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"alloc", (int64_t)(sizeof("alloc") - 1)), slim_allocation_region);
(void)slim_v_matches_alloc;
{
bool slim_v_matches_io = {0};
slim_v_matches_io = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"io", (int64_t)(sizeof("io") - 1)), slim_allocation_region);
(void)slim_v_matches_io;
{
bool slim_v_matches_partial = {0};
slim_v_matches_partial = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"partial", (int64_t)(sizeof("partial") - 1)), slim_allocation_region);
(void)slim_v_matches_partial;
{
bool slim_v_selected = {0};
if (slim_v_alloc_effect) {
slim_v_selected = slim_v_matches_alloc;
}
else {
if (slim_v_io_effect) {
slim_v_selected = slim_v_matches_io;
}
else {
slim_v_selected = slim_v_matches_partial;
}
}
(void)slim_v_selected;
if (slim_v_selected) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_check_builtin_requires_effect(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, int64_t slim_v_effect_code, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_callee;
(void)slim_v_effect_code;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
bool slim_v_alloc_effect = {0};
slim_v_alloc_effect = slim_v_effect_code == INT64_C(1);
(void)slim_v_alloc_effect;
{
bool slim_v_io_effect = {0};
slim_v_io_effect = slim_v_effect_code == INT64_C(2);
(void)slim_v_io_effect;
if (slim_v_alloc_effect) {
{
bool slim_v_vec_new = {0};
slim_v_vec_new = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"vec.new", (int64_t)(sizeof("vec.new") - 1)), slim_allocation_region);
(void)slim_v_vec_new;
{
bool slim_v_vec_push = {0};
slim_v_vec_push = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"vec.push", (int64_t)(sizeof("vec.push") - 1)), slim_allocation_region);
(void)slim_v_vec_push;
{
bool slim_v_arena_new = {0};
slim_v_arena_new = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"arena.new", (int64_t)(sizeof("arena.new") - 1)), slim_allocation_region);
(void)slim_v_arena_new;
{
bool slim_v_arena_add = {0};
slim_v_arena_add = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"arena.add", (int64_t)(sizeof("arena.add") - 1)), slim_allocation_region);
(void)slim_v_arena_add;
{
bool slim_v_read_file = {0};
slim_v_read_file = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.read-file", (int64_t)(sizeof("io.read-file") - 1)), slim_allocation_region);
(void)slim_v_read_file;
{
bool slim_v_first = {0};
slim_v_first = slim_v_vec_new || slim_v_vec_push;
(void)slim_v_first;
{
bool slim_v_second = {0};
slim_v_second = slim_v_arena_new || slim_v_arena_add;
(void)slim_v_second;
{
bool slim_v_storage = {0};
slim_v_storage = slim_v_first || slim_v_second;
(void)slim_v_storage;
slim_result = slim_v_storage || slim_v_read_file;
}
}
}
}
}
}
}
}
}
else {
if (slim_v_io_effect) {
{
bool slim_v_print_i64 = {0};
slim_v_print_i64 = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.print-i64", (int64_t)(sizeof("io.print-i64") - 1)), slim_allocation_region);
(void)slim_v_print_i64;
{
bool slim_v_print_bytes = {0};
slim_v_print_bytes = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.print-bytes", (int64_t)(sizeof("io.print-bytes") - 1)), slim_allocation_region);
(void)slim_v_print_bytes;
{
bool slim_v_println = {0};
slim_v_println = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.println", (int64_t)(sizeof("io.println") - 1)), slim_allocation_region);
(void)slim_v_println;
{
bool slim_v_read_file = {0};
slim_v_read_file = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.read-file", (int64_t)(sizeof("io.read-file") - 1)), slim_allocation_region);
(void)slim_v_read_file;
{
bool slim_v_first = {0};
slim_v_first = slim_v_print_i64 || slim_v_print_bytes;
(void)slim_v_first;
{
bool slim_v_second = {0};
slim_v_second = slim_v_println || slim_v_read_file;
(void)slim_v_second;
slim_result = slim_v_first || slim_v_second;
}
}
}
}
}
}
}
else {
slim_result = false;
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_find_function_effects(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_callee_start, int64_t slim_v_callee_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_callee_start;
(void)slim_v_callee_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
bool slim_v_function_item = {0};
slim_v_function_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_item;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_start;
{
int64_t slim_v_name_end = {0};
slim_v_name_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_end;
{
bool slim_v_same_name = {0};
slim_v_same_name = slim_fn_syntax_spans_equal(slim_v_source, slim_v_name_start, slim_v_name_end, slim_v_callee_start, slim_v_callee_end, slim_allocation_region);
(void)slim_v_same_name;
{
bool slim_v_found = {0};
slim_v_found = slim_v_function_item && slim_v_same_name;
(void)slim_v_found;
if (slim_v_found) {
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_type = {0};
slim_v_return_type = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_type;
slim_result = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_return_type, slim_allocation_region);
}
}
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_check_call_requires_effect(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_callee, int64_t slim_v_effect_code, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_callee;
(void)slim_v_effect_code;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
bool slim_v_builtin = {0};
slim_v_builtin = slim_fn_check_builtin_requires_effect(slim_v_source, slim_v_tokens, slim_v_callee, slim_v_effect_code, slim_allocation_region);
(void)slim_v_builtin;
if (slim_v_builtin) {
slim_result = true;
}
else {
{
int64_t slim_v_callee_start = {0};
slim_v_callee_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_callee_start;
{
int64_t slim_v_callee_end = {0};
slim_v_callee_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_callee_end;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_check_find_function_effects(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_callee_start, slim_v_callee_end, slim_allocation_region);
(void)slim_v_effects;
{
bool slim_v_found = {0};
slim_v_found = slim_v_effects >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
{
int64_t slim_v_first_effect = {0};
slim_v_first_effect = slim_i64_add(slim_v_effects, INT64_C(2));
(void)slim_v_first_effect;
slim_result = slim_fn_check_effect_list_has(slim_v_source, slim_v_tokens, slim_v_first_effect, slim_v_effect_code, slim_allocation_region);
}
}
else {
slim_result = false;
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_check_call_missing_effect(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_declared_effects, int64_t slim_v_callee, int64_t slim_v_effect_code, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_declared_effects;
(void)slim_v_callee;
(void)slim_v_effect_code;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_effect_code > INT64_C(3);
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
bool slim_v_required = {0};
slim_v_required = slim_fn_check_call_requires_effect(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_callee, slim_v_effect_code, slim_allocation_region);
(void)slim_v_required;
{
int64_t slim_v_first_declared = {0};
slim_v_first_declared = slim_i64_add(slim_v_declared_effects, INT64_C(2));
(void)slim_v_first_declared;
{
bool slim_v_declared = {0};
slim_v_declared = slim_fn_check_effect_list_has(slim_v_source, slim_v_tokens, slim_v_first_declared, slim_v_effect_code, slim_allocation_region);
(void)slim_v_declared;
{
bool slim_v_not_declared = {0};
slim_v_not_declared = !slim_v_declared;
(void)slim_v_not_declared;
{
bool slim_v_missing = {0};
slim_v_missing = slim_v_required && slim_v_not_declared;
(void)slim_v_missing;
if (slim_v_missing) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_effect_code, INT64_C(1));
(void)slim_v_next;
slim_v_effect_code = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_find_missing_effect_in_span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_declared_effects, int64_t slim_v_cursor, int64_t slim_v_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_declared_effects;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_call_form = {0};
if (slim_v_form) {
slim_v_call_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"call", (int64_t)(sizeof("call") - 1)), slim_allocation_region);
}
else {
slim_v_call_form = false;
}
(void)slim_v_call_form;
{
bool slim_v_missing = {0};
if (slim_v_call_form) {
{
int64_t slim_v_callee = {0};
slim_v_callee = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_callee;
slim_v_missing = slim_fn_check_call_missing_effect(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_declared_effects, slim_v_callee, INT64_C(1), slim_allocation_region);
}
}
else {
slim_v_missing = false;
}
(void)slim_v_missing;
if (slim_v_missing) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_find_missing_effect_items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_declarations;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_declaration_count = {0};
slim_v_declaration_count = ((*slim_v_declarations)).len;
(void)slim_v_declaration_count;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_declaration_count;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
Slim_type_ir_Declaration slim_v_declaration = {0};
slim_v_declaration = ((Slim_type_ir_Declaration *)((*slim_v_declarations)).data)[slim_vec_check_index(&((*slim_v_declarations)), slim_v_index)];
(void)slim_v_declaration;
{
int64_t slim_v_cursor = {0};
slim_v_cursor = slim_v_declaration.slim_field_form;
(void)slim_v_cursor;
{
bool slim_v_function_item = {0};
slim_v_function_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_item;
{
int64_t slim_v_missing = {0};
if (slim_v_function_item) {
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_type = {0};
slim_v_return_type = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_type;
{
int64_t slim_v_declared_effects = {0};
slim_v_declared_effects = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_return_type, slim_allocation_region);
(void)slim_v_declared_effects;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_declared_effects, slim_allocation_region);
(void)slim_v_body;
{
int64_t slim_v_body_end = {0};
slim_v_body_end = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_end;
slim_v_missing = slim_fn_check_find_missing_effect_in_span(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_declared_effects, slim_v_body, slim_v_body_end, slim_allocation_region);
}
}
}
}
}
}
else {
slim_v_missing = INT64_C(-1);
}
(void)slim_v_missing;
{
bool slim_v_found = {0};
slim_v_found = slim_v_missing >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_missing;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_check_path_effects(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_typed = {0};
slim_v_typed = slim_fn_check_check_path_typed(slim_v_input, slim_v_tokens, slim_v_declarations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
bool slim_v_typed_error = {0};
slim_v_typed_error = slim_v_typed > INT64_C(0);
(void)slim_v_typed_error;
if (slim_v_typed_error) {
slim_result = slim_v_typed;
}
else {
{
int64_t slim_v_missing = {0};
slim_v_missing = slim_fn_check_find_missing_effect_items(slim_v_input, slim_v_tokens, INT64_C(3), slim_v_declarations, INT64_C(0), slim_allocation_region);
(void)slim_v_missing;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_missing >= INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_missing, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_missing, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_closing = {0};
slim_v_closing = slim_i64_sub(slim_v_next, INT64_C(1));
(void)slim_v_closing;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_closing, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_check_report_diagnostic(slim_bytes_static((const uint8_t *)"E0343", (int64_t)(sizeof("E0343") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
else {
slim_result = INT64_C(0);
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_find_overqualified(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_atom = {0};
slim_v_atom = slim_v_kind == INT64_C(2);
(void)slim_v_atom;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_end;
{
int64_t slim_v_slashes = {0};
if (slim_v_atom) {
slim_v_slashes = slim_fn_syntax_span_count_byte(slim_v_source, slim_v_start, slim_v_end, INT64_C(47), INT64_C(0), slim_allocation_region);
}
else {
slim_v_slashes = INT64_C(0);
}
(void)slim_v_slashes;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_slashes > INT64_C(1);
(void)slim_v_invalid;
if (slim_v_invalid) {
slim_result = slim_v_index;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_check_path_project(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_core = {0};
slim_v_core = slim_fn_check_check_path_full(slim_v_input, slim_v_tokens, slim_v_declarations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_core;
{
bool slim_v_core_error = {0};
slim_v_core_error = slim_v_core > INT64_C(0);
(void)slim_v_core_error;
if (slim_v_core_error) {
slim_result = slim_v_core;
}
else {
{
int64_t slim_v_token_count = {0};
slim_v_token_count = ((*slim_v_tokens)).len;
(void)slim_v_token_count;
{
int64_t slim_v_invalid_token = {0};
slim_v_invalid_token = slim_fn_check_find_overqualified(slim_v_input, slim_v_tokens, INT64_C(0), slim_v_token_count, slim_allocation_region);
(void)slim_v_invalid_token;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_invalid_token >= INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_invalid_token, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_invalid_token, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_check_report_diagnostic(slim_bytes_static((const uint8_t *)"E0228", (int64_t)(sizeof("E0228") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
slim_result = INT64_C(0);
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_result_expr(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_expr;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_expr, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
if (!slim_v_form) {
slim_result = slim_v_expr;
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_expr, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_let_form = {0};
slim_v_let_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"let", (int64_t)(sizeof("let") - 1)), slim_allocation_region);
(void)slim_v_let_form;
if (slim_v_let_form) {
{
int64_t slim_v_type_index = {0};
slim_v_type_index = slim_i64_add(slim_v_expr, INT64_C(3));
(void)slim_v_type_index;
{
int64_t slim_v_value = {0};
slim_v_value = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_type_index, slim_allocation_region);
(void)slim_v_value;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_body;
slim_v_expr = slim_v_body;
goto slim_recur;
}
}
}
}
else {
{
bool slim_v_set_form = {0};
slim_v_set_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"set", (int64_t)(sizeof("set") - 1)), slim_allocation_region);
(void)slim_v_set_form;
if (slim_v_set_form) {
{
int64_t slim_v_value = {0};
slim_v_value = slim_i64_add(slim_v_expr, INT64_C(3));
(void)slim_v_value;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_body;
slim_v_expr = slim_v_body;
goto slim_recur;
}
}
}
else {
slim_result = slim_v_expr;
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_atom_type_kind(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_expr;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_expr, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_string_value = {0};
slim_v_string_value = slim_v_kind == INT64_C(3);
(void)slim_v_string_value;
if (slim_v_string_value) {
slim_result = INT64_C(4);
}
else {
{
bool slim_v_true_value = {0};
slim_v_true_value = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_expr, slim_bytes_static((const uint8_t *)"true", (int64_t)(sizeof("true") - 1)), slim_allocation_region);
(void)slim_v_true_value;
{
bool slim_v_false_value = {0};
slim_v_false_value = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_expr, slim_bytes_static((const uint8_t *)"false", (int64_t)(sizeof("false") - 1)), slim_allocation_region);
(void)slim_v_false_value;
{
bool slim_v_bool_value = {0};
slim_v_bool_value = slim_v_true_value || slim_v_false_value;
(void)slim_v_bool_value;
if (slim_v_bool_value) {
slim_result = INT64_C(1);
}
else {
{
bool slim_v_unit_value = {0};
slim_v_unit_value = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_expr, slim_bytes_static((const uint8_t *)"unit", (int64_t)(sizeof("unit") - 1)), slim_allocation_region);
(void)slim_v_unit_value;
if (slim_v_unit_value) {
slim_result = INT64_C(0);
}
else {
{
bool slim_v_number = {0};
slim_v_number = slim_fn_syntax_first_byte_is_number(slim_v_source, slim_v_tokens, slim_v_expr, slim_allocation_region);
(void)slim_v_number;
if (slim_v_number) {
slim_result = INT64_C(3);
}
else {
slim_result = INT64_C(-1);
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_find_type_mismatch_items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_declaration_count = {0};
slim_v_declaration_count = ((*slim_v_declarations)).len;
(void)slim_v_declaration_count;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_declaration_count;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
Slim_type_ir_Declaration slim_v_declaration = {0};
slim_v_declaration = ((Slim_type_ir_Declaration *)((*slim_v_declarations)).data)[slim_vec_check_index(&((*slim_v_declarations)), slim_v_index)];
(void)slim_v_declaration;
{
int64_t slim_v_cursor = {0};
slim_v_cursor = slim_v_declaration.slim_field_form;
(void)slim_v_cursor;
{
bool slim_v_function_item = {0};
slim_v_function_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_item;
{
int64_t slim_v_mismatch = {0};
if (slim_v_function_item) {
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_type = {0};
slim_v_return_type = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_type;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_return_type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_effects, slim_allocation_region);
(void)slim_v_body;
{
int64_t slim_v_result = {0};
slim_v_result = slim_fn_check_result_expr(slim_v_source, slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_result;
{
int64_t slim_v_expected = {0};
slim_v_expected = slim_fn_syntax_scalar_type_kind(slim_v_source, slim_v_tokens, slim_v_return_type, slim_allocation_region);
(void)slim_v_expected;
{
int64_t slim_v_actual = {0};
slim_v_actual = slim_fn_check_atom_type_kind(slim_v_source, slim_v_tokens, slim_v_result, slim_allocation_region);
(void)slim_v_actual;
{
bool slim_v_expected_known = {0};
slim_v_expected_known = slim_v_expected >= INT64_C(0);
(void)slim_v_expected_known;
{
bool slim_v_actual_known = {0};
slim_v_actual_known = slim_v_actual >= INT64_C(0);
(void)slim_v_actual_known;
{
bool slim_v_known = {0};
slim_v_known = slim_v_expected_known && slim_v_actual_known;
(void)slim_v_known;
{
bool slim_v_same = {0};
slim_v_same = slim_v_expected == slim_v_actual;
(void)slim_v_same;
{
bool slim_v_different = {0};
slim_v_different = !slim_v_same;
(void)slim_v_different;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_known && slim_v_different;
(void)slim_v_invalid;
if (slim_v_invalid) {
slim_v_mismatch = slim_v_result;
}
else {
slim_v_mismatch = INT64_C(-1);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
else {
slim_v_mismatch = INT64_C(-1);
}
(void)slim_v_mismatch;
{
bool slim_v_found = {0};
slim_v_found = slim_v_mismatch >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_mismatch;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_check_path_typed(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_syntax = {0};
slim_v_syntax = slim_fn_check_check_path_project(slim_v_input, slim_v_tokens, slim_v_declarations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_syntax;
{
bool slim_v_syntax_error = {0};
slim_v_syntax_error = slim_v_syntax > INT64_C(0);
(void)slim_v_syntax_error;
if (slim_v_syntax_error) {
slim_result = slim_v_syntax;
}
else {
{
int64_t slim_v_mismatch = {0};
slim_v_mismatch = slim_fn_check_find_type_mismatch_items(slim_v_input, slim_v_tokens, slim_v_declarations, INT64_C(0), slim_allocation_region);
(void)slim_v_mismatch;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_mismatch >= INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_mismatch, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_mismatch, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_check_report_diagnostic(slim_bytes_static((const uint8_t *)"E0344", (int64_t)(sizeof("E0344") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
slim_result = INT64_C(0);
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_find_unterminated(SlimVec * slim_v_tokens, int64_t slim_v_index, int64_t slim_v_length, int64_t slim_v_depth, int64_t slim_v_form_start, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_v_depth;
(void)slim_v_form_start;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
{
bool slim_v_open = {0};
slim_v_open = slim_v_depth > INT64_C(0);
(void)slim_v_open;
if (slim_v_open) {
slim_result = slim_v_form_start;
}
else {
slim_result = INT64_C(-1);
}
}
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_left = {0};
slim_v_left = slim_v_kind == INT64_C(0);
(void)slim_v_left;
{
bool slim_v_right = {0};
slim_v_right = slim_v_kind == INT64_C(1);
(void)slim_v_right;
{
bool slim_v_at_root = {0};
slim_v_at_root = slim_v_depth == INT64_C(0);
(void)slim_v_at_root;
{
bool slim_v_root_left = {0};
slim_v_root_left = slim_v_left && slim_v_at_root;
(void)slim_v_root_left;
{
int64_t slim_v_next_start = {0};
if (slim_v_root_left) {
slim_v_next_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_index, slim_allocation_region);
}
else {
slim_v_next_start = slim_v_form_start;
}
(void)slim_v_next_start;
{
int64_t slim_v_increased = {0};
if (slim_v_left) {
slim_v_increased = slim_i64_add(slim_v_depth, INT64_C(1));
}
else {
slim_v_increased = slim_v_depth;
}
(void)slim_v_increased;
{
int64_t slim_v_next_depth = {0};
if (slim_v_right) {
slim_v_next_depth = slim_i64_sub(slim_v_increased, INT64_C(1));
}
else {
slim_v_next_depth = slim_v_increased;
}
(void)slim_v_next_depth;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
slim_v_depth = slim_v_next_depth;
slim_v_form_start = slim_v_next_start;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static SlimUnit slim_fn_check_append_diagnostic_identity(Slim_type_check_Diagnostic slim_v_diagnostic, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_diagnostic;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimBytes slim_v_code = {0};
slim_v_code = slim_v_diagnostic.slim_field_code;
(void)slim_v_code;
{
int64_t slim_v_start = {0};
slim_v_start = slim_v_diagnostic.slim_field_start;
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_v_diagnostic.slim_field_end;
(void)slim_v_end;
{
SlimUnit slim_v_code_text = {0};
slim_v_code_text = slim_fn_text_append_text(slim_v_output, slim_v_code, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_code_text;
{
SlimUnit slim_v_at = {0};
slim_v_at = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"@", (int64_t)(sizeof("@") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_at;
{
SlimUnit slim_v_start_text = {0};
slim_v_start_text = slim_fn_text_append_nonnegative_i64(slim_v_start, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_start_text;
{
SlimUnit slim_v_colon = {0};
slim_v_colon = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)":", (int64_t)(sizeof(":") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_colon;
{
SlimUnit slim_v_end_text = {0};
slim_v_end_text = slim_fn_text_append_nonnegative_i64(slim_v_end, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_end_text;
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"\n", (int64_t)(sizeof("\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_check_path(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_token_count = {0};
slim_v_token_count = ((*slim_v_tokens)).len;
(void)slim_v_token_count;
{
int64_t slim_v_unterminated = {0};
slim_v_unterminated = slim_fn_check_find_unterminated(slim_v_tokens, INT64_C(0), slim_v_token_count, INT64_C(0), INT64_C(0), slim_allocation_region);
(void)slim_v_unterminated;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_unterminated >= INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
{
int64_t slim_v_input_length = {0};
slim_v_input_length = slim_bytes_len(slim_v_input);
(void)slim_v_input_length;
{
Slim_type_check_Diagnostic slim_v_diagnostic = {0};
{
SlimBytes slim_t_12009 = {0};
slim_t_12009 = slim_bytes_static((const uint8_t *)"E0102", (int64_t)(sizeof("E0102") - 1));
int64_t slim_t_12013 = {0};
slim_t_12013 = slim_v_unterminated;
int64_t slim_t_12017 = {0};
slim_t_12017 = slim_v_input_length;
slim_v_diagnostic = (Slim_type_check_Diagnostic){.slim_field_code = slim_t_12009, .slim_field_start = slim_t_12013, .slim_field_end = slim_t_12017};
}
(void)slim_v_diagnostic;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_rendered = {0};
slim_v_rendered = slim_fn_check_append_diagnostic_identity(slim_v_diagnostic, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_rendered;
{
SlimBytes slim_v_bytes = {0};
slim_v_bytes = slim_bytes_freeze(slim_v_output);
(void)slim_v_bytes;
{
SlimUnit slim_v_shown = {0};
slim_v_shown = slim_print_bytes(slim_v_bytes);
(void)slim_v_shown;
slim_result = INT64_C(1);
}
}
}
}
}
}
}
else {
slim_result = INT64_C(0);
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_last_form_in_list(SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_previous, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_previous;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_previous;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_old_cursor = {0};
slim_v_old_cursor = slim_v_cursor;
(void)slim_v_old_cursor;
slim_v_cursor = slim_v_next;
slim_v_previous = slim_v_old_cursor;
goto slim_recur;
}
}
}
}
}
return slim_result;
}

static bool slim_fn_check_known_expr_head(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_head, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_head;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
bool slim_v_let_form = {0};
slim_v_let_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"let", (int64_t)(sizeof("let") - 1)), slim_allocation_region);
(void)slim_v_let_form;
{
bool slim_v_call_form = {0};
slim_v_call_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"call", (int64_t)(sizeof("call") - 1)), slim_allocation_region);
(void)slim_v_call_form;
{
bool slim_v_make_form = {0};
slim_v_make_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"make", (int64_t)(sizeof("make") - 1)), slim_allocation_region);
(void)slim_v_make_form;
{
bool slim_v_get_form = {0};
slim_v_get_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"get", (int64_t)(sizeof("get") - 1)), slim_allocation_region);
(void)slim_v_get_form;
{
bool slim_v_case_form = {0};
slim_v_case_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"case", (int64_t)(sizeof("case") - 1)), slim_allocation_region);
(void)slim_v_case_form;
{
bool slim_v_match_form = {0};
slim_v_match_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"match", (int64_t)(sizeof("match") - 1)), slim_allocation_region);
(void)slim_v_match_form;
{
bool slim_v_recur_form = {0};
slim_v_recur_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"recur", (int64_t)(sizeof("recur") - 1)), slim_allocation_region);
(void)slim_v_recur_form;
{
bool slim_v_set_form = {0};
slim_v_set_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"set", (int64_t)(sizeof("set") - 1)), slim_allocation_region);
(void)slim_v_set_form;
{
bool slim_v_first = {0};
slim_v_first = slim_v_let_form || slim_v_call_form;
(void)slim_v_first;
{
bool slim_v_second = {0};
slim_v_second = slim_v_make_form || slim_v_get_form;
(void)slim_v_second;
{
bool slim_v_third = {0};
slim_v_third = slim_v_case_form || slim_v_match_form;
(void)slim_v_third;
{
bool slim_v_fourth = {0};
slim_v_fourth = slim_v_recur_form || slim_v_set_form;
(void)slim_v_fourth;
{
bool slim_v_left = {0};
slim_v_left = slim_v_first || slim_v_second;
(void)slim_v_left;
{
bool slim_v_right = {0};
slim_v_right = slim_v_third || slim_v_fourth;
(void)slim_v_right;
slim_result = slim_v_left || slim_v_right;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_find_unknown_pair(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_value, int64_t slim_v_body, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_value;
(void)slim_v_body;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
int64_t slim_v_value_unknown = {0};
slim_v_value_unknown = slim_fn_check_find_unknown_expr(slim_v_source, slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_value_unknown;
{
bool slim_v_found = {0};
slim_v_found = slim_v_value_unknown >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_value_unknown;
}
else {
slim_result = slim_fn_check_find_unknown_expr(slim_v_source, slim_v_tokens, slim_v_body, slim_allocation_region);
}
}
}
return slim_result;
}

static int64_t slim_fn_check_find_unknown_control(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, int64_t slim_v_head, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_expr;
(void)slim_v_head;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_match_form = {0};
slim_v_match_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"match", (int64_t)(sizeof("match") - 1)), slim_allocation_region);
(void)slim_v_match_form;
if (slim_v_match_form) {
{
int64_t slim_v_value = {0};
slim_v_value = slim_i64_add(slim_v_expr, INT64_C(2));
(void)slim_v_value;
{
int64_t slim_v_first_arm = {0};
slim_v_first_arm = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_first_arm;
{
int64_t slim_v_value_unknown = {0};
slim_v_value_unknown = slim_fn_check_find_unknown_expr(slim_v_source, slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_value_unknown;
{
bool slim_v_found = {0};
slim_v_found = slim_v_value_unknown >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_value_unknown;
}
else {
slim_result = slim_fn_check_find_unknown_arms(slim_v_source, slim_v_tokens, slim_v_first_arm, slim_allocation_region);
}
}
}
}
}
}
else {
{
bool slim_v_call_form = {0};
slim_v_call_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"call", (int64_t)(sizeof("call") - 1)), slim_allocation_region);
(void)slim_v_call_form;
{
int64_t slim_v_first = {0};
if (slim_v_call_form) {
slim_v_first = slim_i64_add(slim_v_expr, INT64_C(3));
}
else {
slim_v_first = slim_i64_add(slim_v_expr, INT64_C(2));
}
(void)slim_v_first;
slim_result = slim_fn_check_find_unknown_forms(slim_v_source, slim_v_tokens, slim_v_first, slim_allocation_region);
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_find_unknown_data(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, int64_t slim_v_head, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_expr;
(void)slim_v_head;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_make_form = {0};
slim_v_make_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"make", (int64_t)(sizeof("make") - 1)), slim_allocation_region);
(void)slim_v_make_form;
if (slim_v_make_form) {
{
int64_t slim_v_first_field = {0};
slim_v_first_field = slim_i64_add(slim_v_expr, INT64_C(3));
(void)slim_v_first_field;
slim_result = slim_fn_check_find_unknown_fields(slim_v_source, slim_v_tokens, slim_v_first_field, slim_allocation_region);
}
}
else {
{
bool slim_v_get_form = {0};
slim_v_get_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"get", (int64_t)(sizeof("get") - 1)), slim_allocation_region);
(void)slim_v_get_form;
if (slim_v_get_form) {
{
int64_t slim_v_value = {0};
slim_v_value = slim_i64_add(slim_v_expr, INT64_C(2));
(void)slim_v_value;
slim_result = slim_fn_check_find_unknown_expr(slim_v_source, slim_v_tokens, slim_v_value, slim_allocation_region);
}
}
else {
{
bool slim_v_case_form = {0};
slim_v_case_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"case", (int64_t)(sizeof("case") - 1)), slim_allocation_region);
(void)slim_v_case_form;
if (slim_v_case_form) {
{
int64_t slim_v_first_value = {0};
slim_v_first_value = slim_i64_add(slim_v_expr, INT64_C(4));
(void)slim_v_first_value;
slim_result = slim_fn_check_find_unknown_forms(slim_v_source, slim_v_tokens, slim_v_first_value, slim_allocation_region);
}
}
else {
slim_result = slim_fn_check_find_unknown_control(slim_v_source, slim_v_tokens, slim_v_expr, slim_v_head, slim_allocation_region);
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_find_unknown_known(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, int64_t slim_v_head, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_expr;
(void)slim_v_head;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_let_form = {0};
slim_v_let_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"let", (int64_t)(sizeof("let") - 1)), slim_allocation_region);
(void)slim_v_let_form;
if (slim_v_let_form) {
{
int64_t slim_v_type_index = {0};
slim_v_type_index = slim_i64_add(slim_v_expr, INT64_C(3));
(void)slim_v_type_index;
{
int64_t slim_v_value = {0};
slim_v_value = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_type_index, slim_allocation_region);
(void)slim_v_value;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_body;
slim_result = slim_fn_check_find_unknown_pair(slim_v_source, slim_v_tokens, slim_v_value, slim_v_body, slim_allocation_region);
}
}
}
}
else {
{
bool slim_v_set_form = {0};
slim_v_set_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"set", (int64_t)(sizeof("set") - 1)), slim_allocation_region);
(void)slim_v_set_form;
if (slim_v_set_form) {
{
int64_t slim_v_value = {0};
slim_v_value = slim_i64_add(slim_v_expr, INT64_C(3));
(void)slim_v_value;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_body;
slim_result = slim_fn_check_find_unknown_pair(slim_v_source, slim_v_tokens, slim_v_value, slim_v_body, slim_allocation_region);
}
}
}
else {
slim_result = slim_fn_check_find_unknown_data(slim_v_source, slim_v_tokens, slim_v_expr, slim_v_head, slim_allocation_region);
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_find_unknown_expr(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_expr;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_expr, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_atom = {0};
slim_v_atom = slim_v_kind == INT64_C(2);
(void)slim_v_atom;
{
bool slim_v_string_value = {0};
slim_v_string_value = slim_v_kind == INT64_C(3);
(void)slim_v_string_value;
{
bool slim_v_scalar = {0};
slim_v_scalar = slim_v_atom || slim_v_string_value;
(void)slim_v_scalar;
if (slim_v_scalar) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_expr, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_known = {0};
slim_v_known = slim_fn_check_known_expr_head(slim_v_source, slim_v_tokens, slim_v_head, slim_allocation_region);
(void)slim_v_known;
if (slim_v_known) {
slim_result = slim_fn_check_find_unknown_known(slim_v_source, slim_v_tokens, slim_v_expr, slim_v_head, slim_allocation_region);
}
else {
slim_result = slim_v_head;
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_find_unknown_forms(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_unknown = {0};
slim_v_unknown = slim_fn_check_find_unknown_expr(slim_v_source, slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_unknown;
{
bool slim_v_found = {0};
slim_v_found = slim_v_unknown >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_unknown;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_find_unknown_fields(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_value = {0};
slim_v_value = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_value;
{
int64_t slim_v_unknown = {0};
slim_v_unknown = slim_fn_check_find_unknown_expr(slim_v_source, slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_unknown;
{
bool slim_v_found = {0};
slim_v_found = slim_v_unknown >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_unknown;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_find_unknown_arms(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_first = {0};
slim_v_first = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_first;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_check_last_form_in_list(slim_v_tokens, slim_v_first, slim_v_first, slim_allocation_region);
(void)slim_v_body;
{
int64_t slim_v_unknown = {0};
slim_v_unknown = slim_fn_check_find_unknown_expr(slim_v_source, slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_unknown;
{
bool slim_v_found = {0};
slim_v_found = slim_v_unknown >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_unknown;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_find_unknown_items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_count = {0};
slim_v_count = ((*slim_v_declarations)).len;
(void)slim_v_count;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_count;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
Slim_type_ir_Declaration slim_v_declaration = {0};
slim_v_declaration = ((Slim_type_ir_Declaration *)((*slim_v_declarations)).data)[slim_vec_check_index(&((*slim_v_declarations)), slim_v_index)];
(void)slim_v_declaration;
{
int64_t slim_v_cursor = {0};
slim_v_cursor = slim_v_declaration.slim_field_form;
(void)slim_v_cursor;
{
bool slim_v_function_item = {0};
slim_v_function_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_item;
{
int64_t slim_v_unknown = {0};
if (slim_v_function_item) {
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_type = {0};
slim_v_return_type = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_type;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_return_type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_effects, slim_allocation_region);
(void)slim_v_body;
slim_v_unknown = slim_fn_check_find_unknown_expr(slim_v_source, slim_v_tokens, slim_v_body, slim_allocation_region);
}
}
}
}
}
else {
slim_v_unknown = INT64_C(-1);
}
(void)slim_v_unknown;
{
bool slim_v_found = {0};
slim_v_found = slim_v_unknown >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_unknown;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_report_diagnostic(SlimBytes slim_v_code, int64_t slim_v_start, int64_t slim_v_end, SlimRegion *slim_region) {
(void)slim_v_code;
(void)slim_v_start;
(void)slim_v_end;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
Slim_type_check_Diagnostic slim_v_diagnostic = {0};
{
SlimBytes slim_t_13648 = {0};
slim_t_13648 = slim_v_code;
int64_t slim_t_13652 = {0};
slim_t_13652 = slim_v_start;
int64_t slim_t_13656 = {0};
slim_t_13656 = slim_v_end;
slim_v_diagnostic = (Slim_type_check_Diagnostic){.slim_field_code = slim_t_13648, .slim_field_start = slim_t_13652, .slim_field_end = slim_t_13656};
}
(void)slim_v_diagnostic;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_rendered = {0};
slim_v_rendered = slim_fn_check_append_diagnostic_identity(slim_v_diagnostic, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_rendered;
{
SlimBytes slim_v_bytes = {0};
slim_v_bytes = slim_bytes_freeze(slim_v_output);
(void)slim_v_bytes;
{
SlimUnit slim_v_shown = {0};
slim_v_shown = slim_print_bytes(slim_v_bytes);
(void)slim_v_shown;
slim_result = INT64_C(1);
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_check_check_path_full(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_lexical = {0};
slim_v_lexical = slim_fn_check_check_path(slim_v_input, slim_v_tokens, slim_v_declarations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_lexical;
{
bool slim_v_lexical_error = {0};
slim_v_lexical_error = slim_v_lexical > INT64_C(0);
(void)slim_v_lexical_error;
if (slim_v_lexical_error) {
slim_result = slim_v_lexical;
}
else {
{
int64_t slim_v_unknown = {0};
slim_v_unknown = slim_fn_check_find_unknown_items(slim_v_input, slim_v_tokens, slim_v_declarations, INT64_C(0), slim_allocation_region);
(void)slim_v_unknown;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_unknown >= INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_unknown, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_unknown, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_check_report_diagnostic(slim_bytes_static((const uint8_t *)"E0221", (int64_t)(sizeof("E0221") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
slim_result = INT64_C(0);
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_value_address(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_value, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_value;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_number = {0};
slim_v_number = slim_fn_syntax_first_byte_is_number(slim_v_source, slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_number;
if (slim_v_number) {
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"&(int64_t){", (int64_t)(sizeof("&(int64_t){") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"}", (int64_t)(sizeof("}") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
slim_result = slim_fn_codegen_emit_binding_address(slim_v_source, slim_v_tokens, slim_v_params, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_codegen_emit_vec_set_call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_callee;
(void)slim_v_arguments;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_set_call = {0};
slim_v_set_call = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"vec.set", (int64_t)(sizeof("vec.set") - 1)), slim_allocation_region);
(void)slim_v_set_call;
if (slim_v_set_call) {
{
int64_t slim_v_index = {0};
slim_v_index = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_arguments, slim_allocation_region);
(void)slim_v_index;
{
int64_t slim_v_value = {0};
slim_v_value = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_value;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_vec_set(", (int64_t)(sizeof("slim_vec_set(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_vector_address = {0};
slim_v_vector_address = slim_fn_codegen_emit_binding_address(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_vector_address;
{
SlimUnit slim_v_first_comma = {0};
slim_v_first_comma = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)", ", (int64_t)(sizeof(", ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_first_comma;
{
SlimUnit slim_v_index_value = {0};
slim_v_index_value = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_index_value;
{
SlimUnit slim_v_second_comma = {0};
slim_v_second_comma = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)", ", (int64_t)(sizeof(", ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_second_comma;
{
SlimUnit slim_v_value_address = {0};
slim_v_value_address = slim_fn_codegen_emit_value_address(slim_v_source, slim_v_tokens, slim_v_params, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_value_address;
{
SlimUnit slim_v_closed = {0};
slim_v_closed = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"); ", (int64_t)(sizeof("); ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_closed;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_emit_destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = (SlimUnit){0};\n", (int64_t)(sizeof(" = (SlimUnit){0};\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = true;
}
}
}
}
}
}
}
}
}
}
}
}
else {
slim_result = false;
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_codegen_emit_arena_call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_type, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_callee;
(void)slim_v_arguments;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_destination_type;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_new_call = {0};
slim_v_new_call = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"arena.new", (int64_t)(sizeof("arena.new") - 1)), slim_allocation_region);
(void)slim_v_new_call;
if (slim_v_new_call) {
{
int64_t slim_v_inner_type = {0};
slim_v_inner_type = slim_i64_add(slim_v_destination_type, INT64_C(2));
(void)slim_v_inner_type;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_emit_destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = slim_vec_new(sizeof(", (int64_t)(sizeof(" = slim_vec_new(sizeof(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_emit_type(slim_v_source, slim_v_tokens, slim_v_inner_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"), slim_allocation_region);\n", (int64_t)(sizeof("), slim_allocation_region);\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = true;
}
}
}
}
}
}
else {
{
bool slim_v_add_call = {0};
slim_v_add_call = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"arena.add", (int64_t)(sizeof("arena.add") - 1)), slim_allocation_region);
(void)slim_v_add_call;
if (slim_v_add_call) {
{
int64_t slim_v_value = {0};
slim_v_value = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_arguments, slim_allocation_region);
(void)slim_v_value;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_emit_destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = 0;\nif (!slim_arena_add(", (int64_t)(sizeof(" = 0;\nif (!slim_arena_add(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_arena_address = {0};
slim_v_arena_address = slim_fn_codegen_emit_binding_address(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_arena_address;
{
SlimUnit slim_v_comma = {0};
slim_v_comma = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)", ", (int64_t)(sizeof(", ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_comma;
{
SlimUnit slim_v_value_address = {0};
slim_v_value_address = slim_fn_codegen_emit_value_address(slim_v_source, slim_v_tokens, slim_v_params, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_value_address;
{
SlimUnit slim_v_output_comma = {0};
slim_v_output_comma = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)", &", (int64_t)(sizeof(", &") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_output_comma;
{
SlimUnit slim_v_output_dest = {0};
slim_v_output_dest = slim_fn_codegen_emit_destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_output_dest;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)")) goto slim_allocation_failed;\n", (int64_t)(sizeof(")) goto slim_allocation_failed;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = true;
}
}
}
}
}
}
}
}
}
}
else {
{
bool slim_v_get_call = {0};
slim_v_get_call = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"arena.get", (int64_t)(sizeof("arena.get") - 1)), slim_allocation_region);
(void)slim_v_get_call;
if (slim_v_get_call) {
{
int64_t slim_v_identity = {0};
slim_v_identity = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_arguments, slim_allocation_region);
(void)slim_v_identity;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_emit_destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_cast_prefix = {0};
slim_v_cast_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = ((", (int64_t)(sizeof(" = ((") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_cast_prefix;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_emit_type(slim_v_source, slim_v_tokens, slim_v_destination_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_cast_suffix = {0};
slim_v_cast_suffix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" *)(", (int64_t)(sizeof(" *)(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_cast_suffix;
{
SlimUnit slim_v_arena_data = {0};
slim_v_arena_data = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_arena_data;
{
SlimUnit slim_v_check_prefix = {0};
slim_v_check_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)").data)[slim_vec_check_index(&(", (int64_t)(sizeof(").data)[slim_vec_check_index(&(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_check_prefix;
{
SlimUnit slim_v_arena_check = {0};
slim_v_arena_check = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_arena_check;
{
SlimUnit slim_v_comma = {0};
slim_v_comma = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"), ", (int64_t)(sizeof("), ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_comma;
{
SlimUnit slim_v_identity_value = {0};
slim_v_identity_value = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_identity, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_identity_value;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)")];\n", (int64_t)(sizeof(")];\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = true;
}
}
}
}
}
}
}
}
}
}
}
}
else {
slim_result = false;
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_codegen_emit_collection_call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_type, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_callee;
(void)slim_v_arguments;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_destination_type;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_set_call = {0};
slim_v_set_call = slim_fn_codegen_emit_vec_set_call(slim_v_source, slim_v_tokens, slim_v_params, slim_v_callee, slim_v_arguments, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_set_call;
if (slim_v_set_call) {
slim_result = true;
}
else {
{
bool slim_v_vector_call = {0};
slim_v_vector_call = slim_fn_codegen_emit_vector_call(slim_v_source, slim_v_tokens, slim_v_params, slim_v_callee, slim_v_arguments, slim_v_destination, slim_v_result, slim_v_destination_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_vector_call;
if (slim_v_vector_call) {
slim_result = true;
}
else {
slim_result = slim_fn_codegen_emit_arena_call(slim_v_source, slim_v_tokens, slim_v_params, slim_v_callee, slim_v_arguments, slim_v_destination, slim_v_result, slim_v_destination_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_let_full(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_type, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_params;
(void)slim_v_expr;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_destination_type;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_expr, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_type_index = {0};
slim_v_type_index = slim_i64_add(slim_v_expr, INT64_C(3));
(void)slim_v_type_index;
{
int64_t slim_v_value = {0};
slim_v_value = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_type_index, slim_allocation_region);
(void)slim_v_value;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_body;
{
SlimUnit slim_v_open = {0};
slim_v_open = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"{\n", (int64_t)(sizeof("{\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_open;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_emit_type(slim_v_source, slim_v_tokens, slim_v_type_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_emit_source_name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_initialized = {0};
slim_v_initialized = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = {0};\n", (int64_t)(sizeof(" = {0};\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_initialized;
{
SlimUnit slim_v_value_emitted = {0};
slim_v_value_emitted = slim_fn_codegen_emit_expr_full(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_params, slim_v_value, slim_v_name, false, slim_v_type_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_value_emitted;
{
SlimUnit slim_v_unused = {0};
slim_v_unused = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"(void)", (int64_t)(sizeof("(void)") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_unused;
{
SlimUnit slim_v_unused_name = {0};
slim_v_unused_name = slim_fn_codegen_emit_source_name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_unused_name;
{
SlimUnit slim_v_unused_end = {0};
slim_v_unused_end = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_unused_end;
{
SlimUnit slim_v_body_emitted = {0};
slim_v_body_emitted = slim_fn_codegen_emit_expr_full(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_params, slim_v_body, slim_v_destination, slim_v_result, slim_v_destination_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_body_emitted;
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"}\n", (int64_t)(sizeof("}\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_match_full(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_type, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_params;
(void)slim_v_expr;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_destination_type;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_value = {0};
slim_v_value = slim_i64_add(slim_v_expr, INT64_C(2));
(void)slim_v_value;
{
int64_t slim_v_first_arm = {0};
slim_v_first_arm = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_first_arm;
{
int64_t slim_v_first_pattern = {0};
slim_v_first_pattern = slim_i64_add(slim_v_first_arm, INT64_C(1));
(void)slim_v_first_pattern;
{
bool slim_v_first_true = {0};
slim_v_first_true = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_first_pattern, slim_bytes_static((const uint8_t *)"true", (int64_t)(sizeof("true") - 1)), slim_allocation_region);
(void)slim_v_first_true;
{
bool slim_v_first_false = {0};
slim_v_first_false = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_first_pattern, slim_bytes_static((const uint8_t *)"false", (int64_t)(sizeof("false") - 1)), slim_allocation_region);
(void)slim_v_first_false;
{
bool slim_v_boolean_match = {0};
slim_v_boolean_match = slim_v_first_true || slim_v_first_false;
(void)slim_v_boolean_match;
if (slim_v_boolean_match) {
slim_result = slim_fn_codegen_emit_boolean_match(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_params, slim_v_value, slim_v_first_arm, slim_v_destination, slim_v_result, slim_v_destination_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_result = slim_fn_codegen_emit_variant_match(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_params, slim_v_value, slim_v_first_arm, slim_v_destination, slim_v_result, slim_v_destination_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_set_full(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_type, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_params;
(void)slim_v_expr;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_destination_type;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_expr, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_value = {0};
slim_v_value = slim_i64_add(slim_v_expr, INT64_C(3));
(void)slim_v_value;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_body;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_emit_source_name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = ", (int64_t)(sizeof(" = ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_assigned = {0};
slim_v_assigned = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_assigned;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = slim_fn_codegen_emit_expr_full(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_params, slim_v_body, slim_v_destination, slim_v_result, slim_v_destination_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_expr_full(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_type, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_params;
(void)slim_v_expr;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_destination_type;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_expr, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
if (!slim_v_form) {
slim_result = slim_fn_codegen_emit_atom_assignment(slim_v_source, slim_v_tokens, slim_v_params, slim_v_expr, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_expr, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_let_form = {0};
slim_v_let_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"let", (int64_t)(sizeof("let") - 1)), slim_allocation_region);
(void)slim_v_let_form;
if (slim_v_let_form) {
slim_result = slim_fn_codegen_emit_let_full(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_params, slim_v_expr, slim_v_destination, slim_v_result, slim_v_destination_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_call_form = {0};
slim_v_call_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"call", (int64_t)(sizeof("call") - 1)), slim_allocation_region);
(void)slim_v_call_form;
if (slim_v_call_form) {
slim_result = slim_fn_codegen_emit_call(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_params, slim_v_expr, slim_v_destination, slim_v_result, slim_v_destination_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_match_form = {0};
slim_v_match_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"match", (int64_t)(sizeof("match") - 1)), slim_allocation_region);
(void)slim_v_match_form;
if (slim_v_match_form) {
slim_result = slim_fn_codegen_emit_match_full(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_params, slim_v_expr, slim_v_destination, slim_v_result, slim_v_destination_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_recur_form = {0};
slim_v_recur_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"recur", (int64_t)(sizeof("recur") - 1)), slim_allocation_region);
(void)slim_v_recur_form;
if (slim_v_recur_form) {
{
int64_t slim_v_arguments = {0};
slim_v_arguments = slim_i64_add(slim_v_expr, INT64_C(2));
(void)slim_v_arguments;
{
int64_t slim_v_first_param = {0};
slim_v_first_param = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_first_param;
slim_result = slim_fn_codegen_emit_recur_assignments(slim_v_source, slim_v_tokens, slim_v_params, slim_v_first_param, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
{
bool slim_v_set_form = {0};
slim_v_set_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"set", (int64_t)(sizeof("set") - 1)), slim_allocation_region);
(void)slim_v_set_form;
if (slim_v_set_form) {
slim_result = slim_fn_codegen_emit_set_full(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_params, slim_v_expr, slim_v_destination, slim_v_result, slim_v_destination_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_make_form = {0};
slim_v_make_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"make", (int64_t)(sizeof("make") - 1)), slim_allocation_region);
(void)slim_v_make_form;
if (slim_v_make_form) {
slim_result = slim_fn_codegen_emit_make(slim_v_source, slim_v_tokens, slim_v_params, slim_v_expr, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_get_form = {0};
slim_v_get_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"get", (int64_t)(sizeof("get") - 1)), slim_allocation_region);
(void)slim_v_get_form;
if (slim_v_get_form) {
slim_result = slim_fn_codegen_emit_get(slim_v_source, slim_v_tokens, slim_v_params, slim_v_expr, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_case_form = {0};
slim_v_case_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"case", (int64_t)(sizeof("case") - 1)), slim_allocation_region);
(void)slim_v_case_form;
if (slim_v_case_form) {
slim_result = slim_fn_codegen_emit_case_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_expr, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_rt_trap(\"unsupported bootstrap expression\");\n", (int64_t)(sizeof("slim_rt_trap(\"unsupported bootstrap expression\");\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_source_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_v_", (int64_t)(sizeof("slim_v_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
slim_result = slim_fn_text_append_token(slim_v_source, slim_v_tokens, slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_function_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_fn_", (int64_t)(sizeof("slim_fn_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
slim_result = slim_fn_text_append_token(slim_v_source, slim_v_tokens, slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_named_type_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"Slim_type_", (int64_t)(sizeof("Slim_type_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
slim_result = slim_fn_text_append_token(slim_v_source, slim_v_tokens, slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_field_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_field_", (int64_t)(sizeof("slim_field_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
slim_result = slim_fn_text_append_token(slim_v_source, slim_v_tokens, slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_case_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_case_", (int64_t)(sizeof("slim_case_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
slim_result = slim_fn_text_append_token(slim_v_source, slim_v_tokens, slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_tag_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_variant, int64_t slim_v_case_name, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_variant;
(void)slim_v_case_name;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"SLIM_TAG_", (int64_t)(sizeof("SLIM_TAG_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_variant_name = {0};
slim_v_variant_name = slim_fn_text_append_token(slim_v_source, slim_v_tokens, slim_v_variant, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_variant_name;
{
SlimUnit slim_v_separator = {0};
slim_v_separator = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"_", (int64_t)(sizeof("_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_separator;
slim_result = slim_fn_text_append_token(slim_v_source, slim_v_tokens, slim_v_case_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_type(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_is_unit = {0};
slim_v_is_unit = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_index, slim_bytes_static((const uint8_t *)"Unit", (int64_t)(sizeof("Unit") - 1)), slim_allocation_region);
(void)slim_v_is_unit;
if (slim_v_is_unit) {
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"SlimUnit", (int64_t)(sizeof("SlimUnit") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_is_bool = {0};
slim_v_is_bool = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_index, slim_bytes_static((const uint8_t *)"Bool", (int64_t)(sizeof("Bool") - 1)), slim_allocation_region);
(void)slim_v_is_bool;
if (slim_v_is_bool) {
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"bool", (int64_t)(sizeof("bool") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_is_u8 = {0};
slim_v_is_u8 = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_index, slim_bytes_static((const uint8_t *)"U8", (int64_t)(sizeof("U8") - 1)), slim_allocation_region);
(void)slim_v_is_u8;
if (slim_v_is_u8) {
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"uint8_t", (int64_t)(sizeof("uint8_t") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_is_i64 = {0};
slim_v_is_i64 = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_index, slim_bytes_static((const uint8_t *)"I64", (int64_t)(sizeof("I64") - 1)), slim_allocation_region);
(void)slim_v_is_i64;
if (slim_v_is_i64) {
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"int64_t", (int64_t)(sizeof("int64_t") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_is_bytes = {0};
slim_v_is_bytes = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_index, slim_bytes_static((const uint8_t *)"Bytes", (int64_t)(sizeof("Bytes") - 1)), slim_allocation_region);
(void)slim_v_is_bytes;
if (slim_v_is_bytes) {
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"SlimBytes", (int64_t)(sizeof("SlimBytes") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_composite = {0};
slim_v_composite = slim_v_kind == INT64_C(0);
(void)slim_v_composite;
if (slim_v_composite) {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_id_type = {0};
slim_v_id_type = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"Id", (int64_t)(sizeof("Id") - 1)), slim_allocation_region);
(void)slim_v_id_type;
if (slim_v_id_type) {
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"SlimId", (int64_t)(sizeof("SlimId") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"SlimVec", (int64_t)(sizeof("SlimVec") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
else {
slim_result = slim_fn_codegen_emit_named_type_name(slim_v_source, slim_v_tokens, slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_forward_types(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"\n", (int64_t)(sizeof("\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_record_item = {0};
slim_v_record_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"record", (int64_t)(sizeof("record") - 1)), slim_allocation_region);
(void)slim_v_record_item;
{
bool slim_v_variant_item = {0};
slim_v_variant_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"variant", (int64_t)(sizeof("variant") - 1)), slim_allocation_region);
(void)slim_v_variant_item;
{
bool slim_v_data_item = {0};
slim_v_data_item = slim_v_record_item || slim_v_variant_item;
(void)slim_v_data_item;
{
SlimUnit slim_v_emitted = {0};
if (slim_v_data_item) {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"typedef struct ", (int64_t)(sizeof("typedef struct ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_first_name = {0};
slim_v_first_name = slim_fn_codegen_emit_named_type_name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_first_name;
{
SlimUnit slim_v_separator = {0};
slim_v_separator = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_separator;
{
SlimUnit slim_v_second_name = {0};
slim_v_second_name = slim_fn_codegen_emit_named_type_name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_second_name;
slim_v_emitted = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
else {
slim_v_emitted = (SlimUnit){0};
}
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_record_fields(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_first;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
if (slim_v_first) {
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"SlimUnit slim_unit;\n", (int64_t)(sizeof("SlimUnit slim_unit;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_result = (SlimUnit){0};
}
}
else {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_name;
{
int64_t slim_v_type_index = {0};
slim_v_type_index = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_type_index;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_emit_type(slim_v_source, slim_v_tokens, slim_v_type_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_separated = {0};
slim_v_separated = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_separated;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_emit_field_name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_terminated = {0};
slim_v_terminated = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_terminated;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
slim_v_first = false;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_record(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_item, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_item;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_item, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_fields = {0};
slim_v_fields = slim_i64_add(slim_v_item, INT64_C(3));
(void)slim_v_fields;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"struct ", (int64_t)(sizeof("struct ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_emit_named_type_name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_opened = {0};
slim_v_opened = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" {\n", (int64_t)(sizeof(" {\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_opened;
{
int64_t slim_v_first_field = {0};
slim_v_first_field = slim_i64_add(slim_v_fields, INT64_C(1));
(void)slim_v_first_field;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_emit_record_fields(slim_v_source, slim_v_tokens, slim_v_first_field, true, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"};\n\n", (int64_t)(sizeof("};\n\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_variant_tags(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_variant_name, int64_t slim_v_cursor, int64_t slim_v_tag, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_variant_name;
(void)slim_v_cursor;
(void)slim_v_tag;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_case_name = {0};
slim_v_case_name = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_case_name;
{
SlimUnit slim_v_define = {0};
slim_v_define = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"#define ", (int64_t)(sizeof("#define ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_define;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_emit_tag_name(slim_v_source, slim_v_tokens, slim_v_variant_name, slim_v_case_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_cast = {0};
slim_v_cast = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" ((int32_t)", (int64_t)(sizeof(" ((int32_t)") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_cast;
{
SlimUnit slim_v_number = {0};
slim_v_number = slim_fn_text_append_nonnegative_i64(slim_v_tag, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_number;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)")\n", (int64_t)(sizeof(")\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_next_tag = {0};
slim_v_next_tag = slim_i64_add(slim_v_tag, INT64_C(1));
(void)slim_v_next_tag;
slim_v_cursor = slim_v_next;
slim_v_tag = slim_v_next_tag;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_variant_payload_fields(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_field, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_field;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_emit_type(slim_v_source, slim_v_tokens, slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" slim_", (int64_t)(sizeof(" slim_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_number = {0};
slim_v_number = slim_fn_text_append_nonnegative_i64(slim_v_field, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_number;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_next_field = {0};
slim_v_next_field = slim_i64_add(slim_v_field, INT64_C(1));
(void)slim_v_next_field;
slim_v_cursor = slim_v_next;
slim_v_field = slim_v_next_field;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_variant_payloads(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_case_name = {0};
slim_v_case_name = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_case_name;
{
int64_t slim_v_payload = {0};
slim_v_payload = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_payload;
{
int64_t slim_v_payload_kind = {0};
slim_v_payload_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_payload, slim_allocation_region);
(void)slim_v_payload_kind;
{
bool slim_v_empty = {0};
slim_v_empty = slim_v_payload_kind == INT64_C(1);
(void)slim_v_empty;
{
SlimUnit slim_v_emitted = {0};
if (slim_v_empty) {
slim_v_emitted = (SlimUnit){0};
}
else {
{
SlimUnit slim_v_opened = {0};
slim_v_opened = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"struct {\n", (int64_t)(sizeof("struct {\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_opened;
{
SlimUnit slim_v_fields = {0};
slim_v_fields = slim_fn_codegen_emit_variant_payload_fields(slim_v_source, slim_v_tokens, slim_v_payload, INT64_C(0), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_fields;
{
SlimUnit slim_v_closed = {0};
slim_v_closed = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"} ", (int64_t)(sizeof("} ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_closed;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_emit_case_name(slim_v_source, slim_v_tokens, slim_v_case_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
slim_v_emitted = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_variant(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_item, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_item;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_item, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_cases = {0};
slim_v_cases = slim_i64_add(slim_v_item, INT64_C(3));
(void)slim_v_cases;
{
int64_t slim_v_first_case = {0};
slim_v_first_case = slim_i64_add(slim_v_cases, INT64_C(1));
(void)slim_v_first_case;
{
SlimUnit slim_v_tags = {0};
slim_v_tags = slim_fn_codegen_emit_variant_tags(slim_v_source, slim_v_tokens, slim_v_name, slim_v_first_case, INT64_C(0), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tags;
{
SlimUnit slim_v_struct_prefix = {0};
slim_v_struct_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"struct ", (int64_t)(sizeof("struct ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_struct_prefix;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_emit_named_type_name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_opened = {0};
slim_v_opened = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" {\nint32_t tag;\nunion {\nuint8_t slim_unused;\n", (int64_t)(sizeof(" {\nint32_t tag;\nunion {\nuint8_t slim_unused;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_opened;
{
SlimUnit slim_v_payloads = {0};
slim_v_payloads = slim_fn_codegen_emit_variant_payloads(slim_v_source, slim_v_tokens, slim_v_first_case, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_payloads;
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"} data;\n};\n\n", (int64_t)(sizeof("} data;\n};\n\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_data_items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
bool slim_v_record_item = {0};
slim_v_record_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"record", (int64_t)(sizeof("record") - 1)), slim_allocation_region);
(void)slim_v_record_item;
{
SlimUnit slim_v_emitted_record = {0};
if (slim_v_record_item) {
slim_v_emitted_record = slim_fn_codegen_emit_record(slim_v_source, slim_v_tokens, slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_emitted_record = (SlimUnit){0};
}
(void)slim_v_emitted_record;
{
bool slim_v_variant_item = {0};
slim_v_variant_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"variant", (int64_t)(sizeof("variant") - 1)), slim_allocation_region);
(void)slim_v_variant_item;
{
SlimUnit slim_v_emitted_variant = {0};
if (slim_v_variant_item) {
slim_v_emitted_variant = slim_fn_codegen_emit_variant(slim_v_source, slim_v_tokens, slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_emitted_variant = (SlimUnit){0};
}
(void)slim_v_emitted_variant;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_binding_value(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_name, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_name;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_name_is_inout(slim_v_source, slim_v_tokens, slim_v_params, slim_v_name, slim_allocation_region);
(void)slim_v_borrowed;
if (slim_v_borrowed) {
{
SlimUnit slim_v_open = {0};
slim_v_open = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"(*", (int64_t)(sizeof("(*") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_open;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_emit_source_name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
slim_result = slim_fn_codegen_emit_source_name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_binding_address(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_name, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_name;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_name_is_inout(slim_v_source, slim_v_tokens, slim_v_params, slim_v_name, slim_allocation_region);
(void)slim_v_borrowed;
if (slim_v_borrowed) {
slim_result = slim_fn_codegen_emit_source_name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
SlimUnit slim_v_address = {0};
slim_v_address = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"&", (int64_t)(sizeof("&") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_address;
slim_result = slim_fn_codegen_emit_source_name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_value(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_value, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_value;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_string_value = {0};
slim_v_string_value = slim_v_kind == INT64_C(3);
(void)slim_v_string_value;
if (slim_v_string_value) {
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_bytes_static((const uint8_t *)", (int64_t)(sizeof("slim_bytes_static((const uint8_t *)") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_first = {0};
slim_v_first = slim_fn_text_append_token(slim_v_source, slim_v_tokens, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_first;
{
SlimUnit slim_v_middle = {0};
slim_v_middle = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)", (int64_t)(sizeof(", (int64_t)(sizeof(", (int64_t)(sizeof(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_middle;
{
SlimUnit slim_v_second = {0};
slim_v_second = slim_fn_text_append_token(slim_v_source, slim_v_tokens, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_second;
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)") - 1))", (int64_t)(sizeof(") - 1))") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
else {
{
bool slim_v_is_true = {0};
slim_v_is_true = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_value, slim_bytes_static((const uint8_t *)"true", (int64_t)(sizeof("true") - 1)), slim_allocation_region);
(void)slim_v_is_true;
if (slim_v_is_true) {
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"true", (int64_t)(sizeof("true") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_is_false = {0};
slim_v_is_false = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_value, slim_bytes_static((const uint8_t *)"false", (int64_t)(sizeof("false") - 1)), slim_allocation_region);
(void)slim_v_is_false;
if (slim_v_is_false) {
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"false", (int64_t)(sizeof("false") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_is_unit = {0};
slim_v_is_unit = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_value, slim_bytes_static((const uint8_t *)"unit", (int64_t)(sizeof("unit") - 1)), slim_allocation_region);
(void)slim_v_is_unit;
if (slim_v_is_unit) {
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"(SlimUnit){0}", (int64_t)(sizeof("(SlimUnit){0}") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_number = {0};
slim_v_number = slim_fn_syntax_first_byte_is_number(slim_v_source, slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_number;
if (slim_v_number) {
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"INT64_C(", (int64_t)(sizeof("INT64_C(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_literal = {0};
slim_v_literal = slim_fn_text_append_token(slim_v_source, slim_v_tokens, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_literal;
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
slim_result = slim_fn_codegen_emit_binding_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimBytes slim_fn_codegen_runtime_math_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_callee;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimBytes slim_result = {0};
{
bool slim_v_add = {0};
slim_v_add = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.add", (int64_t)(sizeof("i64.add") - 1)), slim_allocation_region);
(void)slim_v_add;
if (slim_v_add) {
slim_result = slim_bytes_static((const uint8_t *)"slim_i64_add", (int64_t)(sizeof("slim_i64_add") - 1));
}
else {
{
bool slim_v_sub = {0};
slim_v_sub = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.sub", (int64_t)(sizeof("i64.sub") - 1)), slim_allocation_region);
(void)slim_v_sub;
if (slim_v_sub) {
slim_result = slim_bytes_static((const uint8_t *)"slim_i64_sub", (int64_t)(sizeof("slim_i64_sub") - 1));
}
else {
{
bool slim_v_mul = {0};
slim_v_mul = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.mul", (int64_t)(sizeof("i64.mul") - 1)), slim_allocation_region);
(void)slim_v_mul;
if (slim_v_mul) {
slim_result = slim_bytes_static((const uint8_t *)"slim_i64_mul", (int64_t)(sizeof("slim_i64_mul") - 1));
}
else {
{
bool slim_v_div = {0};
slim_v_div = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.div", (int64_t)(sizeof("i64.div") - 1)), slim_allocation_region);
(void)slim_v_div;
if (slim_v_div) {
slim_result = slim_bytes_static((const uint8_t *)"slim_i64_div", (int64_t)(sizeof("slim_i64_div") - 1));
}
else {
{
bool slim_v_rem = {0};
slim_v_rem = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.rem", (int64_t)(sizeof("i64.rem") - 1)), slim_allocation_region);
(void)slim_v_rem;
if (slim_v_rem) {
slim_result = slim_bytes_static((const uint8_t *)"slim_i64_rem", (int64_t)(sizeof("slim_i64_rem") - 1));
}
else {
{
bool slim_v_to_u8 = {0};
slim_v_to_u8 = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.to-u8", (int64_t)(sizeof("i64.to-u8") - 1)), slim_allocation_region);
(void)slim_v_to_u8;
if (slim_v_to_u8) {
slim_result = slim_bytes_static((const uint8_t *)"slim_i64_to_u8", (int64_t)(sizeof("slim_i64_to_u8") - 1));
}
else {
slim_result = slim_bytes_static((const uint8_t *)"", (int64_t)(sizeof("") - 1));
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static SlimBytes slim_fn_codegen_runtime_bytes_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_callee;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimBytes slim_result = {0};
{
bool slim_v_bytes_len = {0};
slim_v_bytes_len = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bytes.len", (int64_t)(sizeof("bytes.len") - 1)), slim_allocation_region);
(void)slim_v_bytes_len;
if (slim_v_bytes_len) {
slim_result = slim_bytes_static((const uint8_t *)"slim_bytes_len", (int64_t)(sizeof("slim_bytes_len") - 1));
}
else {
{
bool slim_v_bytes_get = {0};
slim_v_bytes_get = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bytes.get", (int64_t)(sizeof("bytes.get") - 1)), slim_allocation_region);
(void)slim_v_bytes_get;
if (slim_v_bytes_get) {
slim_result = slim_bytes_static((const uint8_t *)"slim_bytes_get", (int64_t)(sizeof("slim_bytes_get") - 1));
}
else {
{
bool slim_v_freeze = {0};
slim_v_freeze = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bytes.freeze", (int64_t)(sizeof("bytes.freeze") - 1)), slim_allocation_region);
(void)slim_v_freeze;
if (slim_v_freeze) {
slim_result = slim_bytes_static((const uint8_t *)"slim_bytes_freeze", (int64_t)(sizeof("slim_bytes_freeze") - 1));
}
else {
slim_result = slim_bytes_static((const uint8_t *)"", (int64_t)(sizeof("") - 1));
}
}
}
}
}
}
return slim_result;
}

static SlimBytes slim_fn_codegen_runtime_io_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_callee;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimBytes slim_result = {0};
{
bool slim_v_print_i64 = {0};
slim_v_print_i64 = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.print-i64", (int64_t)(sizeof("io.print-i64") - 1)), slim_allocation_region);
(void)slim_v_print_i64;
if (slim_v_print_i64) {
slim_result = slim_bytes_static((const uint8_t *)"slim_print_i64", (int64_t)(sizeof("slim_print_i64") - 1));
}
else {
{
bool slim_v_print_bytes = {0};
slim_v_print_bytes = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.print-bytes", (int64_t)(sizeof("io.print-bytes") - 1)), slim_allocation_region);
(void)slim_v_print_bytes;
if (slim_v_print_bytes) {
slim_result = slim_bytes_static((const uint8_t *)"slim_print_bytes", (int64_t)(sizeof("slim_print_bytes") - 1));
}
else {
{
bool slim_v_println = {0};
slim_v_println = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.println", (int64_t)(sizeof("io.println") - 1)), slim_allocation_region);
(void)slim_v_println;
if (slim_v_println) {
slim_result = slim_bytes_static((const uint8_t *)"slim_println", (int64_t)(sizeof("slim_println") - 1));
}
else {
slim_result = slim_bytes_static((const uint8_t *)"", (int64_t)(sizeof("") - 1));
}
}
}
}
}
}
return slim_result;
}

static SlimBytes slim_fn_codegen_runtime_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_callee;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimBytes slim_result = {0};
{
SlimBytes slim_v_math = {0};
slim_v_math = slim_fn_codegen_runtime_math_name(slim_v_source, slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_math;
{
int64_t slim_v_math_length = {0};
slim_v_math_length = slim_bytes_len(slim_v_math);
(void)slim_v_math_length;
{
bool slim_v_has_math = {0};
slim_v_has_math = slim_v_math_length > INT64_C(0);
(void)slim_v_has_math;
if (slim_v_has_math) {
slim_result = slim_v_math;
}
else {
{
SlimBytes slim_v_bytes = {0};
slim_v_bytes = slim_fn_codegen_runtime_bytes_name(slim_v_source, slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_bytes;
{
int64_t slim_v_bytes_length = {0};
slim_v_bytes_length = slim_bytes_len(slim_v_bytes);
(void)slim_v_bytes_length;
{
bool slim_v_has_bytes = {0};
slim_v_has_bytes = slim_v_bytes_length > INT64_C(0);
(void)slim_v_has_bytes;
if (slim_v_has_bytes) {
slim_result = slim_v_bytes;
}
else {
slim_result = slim_fn_codegen_runtime_io_name(slim_v_source, slim_v_tokens, slim_v_callee, slim_allocation_region);
}
}
}
}
}
}
}
}
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_value_arguments(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_cursor;
(void)slim_v_first;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
if (slim_v_first) {
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
slim_v_first = false;
goto slim_recur;
}
}
}
else {
{
SlimUnit slim_v_comma = {0};
slim_v_comma = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)", ", (int64_t)(sizeof(", ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_comma;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
slim_v_first = false;
goto slim_recur;
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_user_arguments(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_current_params, int64_t slim_v_argument, int64_t slim_v_callee_param, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_current_params;
(void)slim_v_argument;
(void)slim_v_callee_param;
(void)slim_v_first;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
{
SlimUnit slim_v_separator = {0};
if (slim_v_first) {
slim_v_separator = (SlimUnit){0};
}
else {
slim_v_separator = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)", ", (int64_t)(sizeof(", ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_separator;
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_allocation_region", (int64_t)(sizeof("slim_allocation_region") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
else {
{
SlimUnit slim_v_separated = {0};
if (slim_v_first) {
slim_v_separated = (SlimUnit){0};
}
else {
slim_v_separated = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)", ", (int64_t)(sizeof(", ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_separated;
{
int64_t slim_v_param_head = {0};
slim_v_param_head = slim_i64_add(slim_v_callee_param, INT64_C(1));
(void)slim_v_param_head;
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_param_head, slim_bytes_static((const uint8_t *)"inout", (int64_t)(sizeof("inout") - 1)), slim_allocation_region);
(void)slim_v_borrowed;
{
SlimUnit slim_v_emitted = {0};
if (slim_v_borrowed) {
slim_v_emitted = slim_fn_codegen_emit_binding_address(slim_v_source, slim_v_tokens, slim_v_current_params, slim_v_argument, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_emitted = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, slim_v_current_params, slim_v_argument, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_emitted;
{
int64_t slim_v_next_argument = {0};
slim_v_next_argument = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_next_argument;
{
int64_t slim_v_next_param = {0};
slim_v_next_param = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_callee_param, slim_allocation_region);
(void)slim_v_next_param;
slim_v_argument = slim_v_next_argument;
slim_v_callee_param = slim_v_next_param;
slim_v_first = false;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_parameters(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_first;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
{
SlimUnit slim_v_separator = {0};
if (slim_v_first) {
slim_v_separator = (SlimUnit){0};
}
else {
slim_v_separator = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)", ", (int64_t)(sizeof(", ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_separator;
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"SlimRegion *slim_region", (int64_t)(sizeof("SlimRegion *slim_region") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
else {
{
SlimUnit slim_v_separated = {0};
if (slim_v_first) {
slim_v_separated = (SlimUnit){0};
}
else {
slim_v_separated = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)", ", (int64_t)(sizeof(", ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_separated;
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"inout", (int64_t)(sizeof("inout") - 1)), slim_allocation_region);
(void)slim_v_borrowed;
{
int64_t slim_v_name = {0};
if (slim_v_borrowed) {
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
}
else {
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(1));
}
(void)slim_v_name;
{
int64_t slim_v_type = {0};
if (slim_v_borrowed) {
slim_v_type = slim_i64_add(slim_v_cursor, INT64_C(3));
}
else {
slim_v_type = slim_i64_add(slim_v_cursor, INT64_C(2));
}
(void)slim_v_type;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_emit_type(slim_v_source, slim_v_tokens, slim_v_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_pointer = {0};
if (slim_v_borrowed) {
slim_v_pointer = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" *", (int64_t)(sizeof(" *") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_pointer = (SlimUnit){0};
}
(void)slim_v_pointer;
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_emit_source_name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
slim_v_first = false;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_prototype(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_item, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_item;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_item, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_item, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_type = {0};
slim_v_return_type = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_type;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"static ", (int64_t)(sizeof("static ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_returned = {0};
slim_v_returned = slim_fn_codegen_emit_type(slim_v_source, slim_v_tokens, slim_v_return_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_returned;
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_emit_function_name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_open = {0};
slim_v_open = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"(", (int64_t)(sizeof("(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_open;
{
int64_t slim_v_first_param = {0};
slim_v_first_param = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_first_param;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_emit_parameters(slim_v_source, slim_v_tokens, slim_v_first_param, true, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)");\n", (int64_t)(sizeof(");\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_prototypes(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"\n", (int64_t)(sizeof("\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_function_item = {0};
slim_v_function_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_item;
{
SlimUnit slim_v_emitted = {0};
if (slim_v_function_item) {
slim_v_emitted = slim_fn_codegen_emit_prototype(slim_v_source, slim_v_tokens, slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_emitted = (SlimUnit){0};
}
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_codegen_contains_atom(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_end, SlimBytes slim_v_text, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_v_text;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_atom = {0};
slim_v_atom = slim_v_kind == INT64_C(2);
(void)slim_v_atom;
{
bool slim_v_same = {0};
if (slim_v_atom) {
slim_v_same = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_cursor, slim_v_text, slim_allocation_region);
}
else {
slim_v_same = false;
}
(void)slim_v_same;
if (slim_v_same) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_parameter_voids(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"(void)slim_region;\n", (int64_t)(sizeof("(void)slim_region;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"inout", (int64_t)(sizeof("inout") - 1)), slim_allocation_region);
(void)slim_v_borrowed;
{
int64_t slim_v_name = {0};
if (slim_v_borrowed) {
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
}
else {
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(1));
}
(void)slim_v_name;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"(void)", (int64_t)(sizeof("(void)") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_emit_source_name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_temp_name(int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_index;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_t_", (int64_t)(sizeof("slim_t_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
slim_result = slim_fn_text_append_nonnegative_i64(slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_destination(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
if (slim_v_result) {
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_result", (int64_t)(sizeof("slim_result") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_temporary = {0};
slim_v_temporary = slim_v_destination < INT64_C(0);
(void)slim_v_temporary;
if (slim_v_temporary) {
{
int64_t slim_v_index = {0};
slim_v_index = slim_i64_sub(INT64_C(0), slim_v_destination);
(void)slim_v_index;
slim_result = slim_fn_codegen_emit_temp_name(slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
else {
slim_result = slim_fn_codegen_emit_source_name(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_recur_assignments(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_param, int64_t slim_v_argument, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_param;
(void)slim_v_argument;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_param, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"goto slim_recur;\n", (int64_t)(sizeof("goto slim_recur;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_param, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"inout", (int64_t)(sizeof("inout") - 1)), slim_allocation_region);
(void)slim_v_borrowed;
{
int64_t slim_v_name = {0};
if (slim_v_borrowed) {
slim_v_name = slim_i64_add(slim_v_param, INT64_C(2));
}
else {
slim_v_name = slim_i64_add(slim_v_param, INT64_C(1));
}
(void)slim_v_name;
{
SlimUnit slim_v_assigned = {0};
if (slim_v_borrowed) {
slim_v_assigned = (SlimUnit){0};
}
else {
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_start;
{
int64_t slim_v_name_end = {0};
slim_v_name_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_end;
{
int64_t slim_v_argument_start = {0};
slim_v_argument_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_argument_start;
{
int64_t slim_v_argument_end = {0};
slim_v_argument_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_argument_end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_spans_equal(slim_v_source, slim_v_name_start, slim_v_name_end, slim_v_argument_start, slim_v_argument_end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_v_assigned = (SlimUnit){0};
}
else {
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_emit_source_name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = ", (int64_t)(sizeof(" = ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_value = {0};
slim_v_value = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_argument, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_value;
slim_v_assigned = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
(void)slim_v_assigned;
{
int64_t slim_v_next_param = {0};
slim_v_next_param = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_param, slim_allocation_region);
(void)slim_v_next_param;
{
int64_t slim_v_next_argument = {0};
slim_v_next_argument = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_next_argument;
slim_v_param = slim_v_next_param;
slim_v_argument = slim_v_next_argument;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_runtime_assignment(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimBytes slim_v_runtime, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_arguments;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_runtime;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_emit_destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = ", (int64_t)(sizeof(" = ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_text_append_text(slim_v_output, slim_v_runtime, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_open = {0};
slim_v_open = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"(", (int64_t)(sizeof("(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_open;
{
SlimUnit slim_v_args = {0};
slim_v_args = slim_fn_codegen_emit_value_arguments(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, true, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_args;
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)");\n", (int64_t)(sizeof(");\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_binary_operator(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimBytes slim_v_operator, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_arguments;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_operator;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_right = {0};
slim_v_right = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_arguments, slim_allocation_region);
(void)slim_v_right;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_emit_destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = ", (int64_t)(sizeof(" = ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_left_value = {0};
slim_v_left_value = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_left_value;
{
SlimUnit slim_v_op = {0};
slim_v_op = slim_fn_text_append_text(slim_v_output, slim_v_operator, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_op;
{
SlimUnit slim_v_right_value = {0};
slim_v_right_value = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_right, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_right_value;
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_codegen_emit_comparison(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_callee;
(void)slim_v_arguments;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_eq = {0};
slim_v_eq = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.eq", (int64_t)(sizeof("i64.eq") - 1)), slim_allocation_region);
(void)slim_v_eq;
if (slim_v_eq) {
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_emit_binary_operator(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_destination, slim_v_result, slim_bytes_static((const uint8_t *)" == ", (int64_t)(sizeof(" == ") - 1)), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = true;
}
}
else {
{
bool slim_v_lt = {0};
slim_v_lt = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.lt", (int64_t)(sizeof("i64.lt") - 1)), slim_allocation_region);
(void)slim_v_lt;
if (slim_v_lt) {
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_emit_binary_operator(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_destination, slim_v_result, slim_bytes_static((const uint8_t *)" < ", (int64_t)(sizeof(" < ") - 1)), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = true;
}
}
else {
{
bool slim_v_le = {0};
slim_v_le = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.le", (int64_t)(sizeof("i64.le") - 1)), slim_allocation_region);
(void)slim_v_le;
if (slim_v_le) {
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_emit_binary_operator(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_destination, slim_v_result, slim_bytes_static((const uint8_t *)" <= ", (int64_t)(sizeof(" <= ") - 1)), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = true;
}
}
else {
{
bool slim_v_gt = {0};
slim_v_gt = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.gt", (int64_t)(sizeof("i64.gt") - 1)), slim_allocation_region);
(void)slim_v_gt;
if (slim_v_gt) {
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_emit_binary_operator(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_destination, slim_v_result, slim_bytes_static((const uint8_t *)" > ", (int64_t)(sizeof(" > ") - 1)), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = true;
}
}
else {
{
bool slim_v_ge = {0};
slim_v_ge = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.ge", (int64_t)(sizeof("i64.ge") - 1)), slim_allocation_region);
(void)slim_v_ge;
if (slim_v_ge) {
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_emit_binary_operator(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_destination, slim_v_result, slim_bytes_static((const uint8_t *)" >= ", (int64_t)(sizeof(" >= ") - 1)), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = true;
}
}
else {
slim_result = false;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_codegen_emit_boolean_call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_callee;
(void)slim_v_arguments;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_and_call = {0};
slim_v_and_call = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bool.and", (int64_t)(sizeof("bool.and") - 1)), slim_allocation_region);
(void)slim_v_and_call;
if (slim_v_and_call) {
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_emit_binary_operator(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_destination, slim_v_result, slim_bytes_static((const uint8_t *)" && ", (int64_t)(sizeof(" && ") - 1)), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = true;
}
}
else {
{
bool slim_v_or_call = {0};
slim_v_or_call = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bool.or", (int64_t)(sizeof("bool.or") - 1)), slim_allocation_region);
(void)slim_v_or_call;
if (slim_v_or_call) {
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_emit_binary_operator(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_destination, slim_v_result, slim_bytes_static((const uint8_t *)" || ", (int64_t)(sizeof(" || ") - 1)), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = true;
}
}
else {
{
bool slim_v_not_call = {0};
slim_v_not_call = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bool.not", (int64_t)(sizeof("bool.not") - 1)), slim_allocation_region);
(void)slim_v_not_call;
if (slim_v_not_call) {
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_emit_destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = !", (int64_t)(sizeof(" = !") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_value = {0};
slim_v_value = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_value;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = true;
}
}
}
}
}
else {
slim_result = false;
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_codegen_emit_u8_conversion(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_callee;
(void)slim_v_arguments;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_conversion = {0};
slim_v_conversion = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"u8.to-i64", (int64_t)(sizeof("u8.to-i64") - 1)), slim_allocation_region);
(void)slim_v_conversion;
if (slim_v_conversion) {
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_emit_destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = (int64_t)", (int64_t)(sizeof(" = (int64_t)") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_value = {0};
slim_v_value = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_value;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = true;
}
}
}
}
}
else {
slim_result = false;
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_codegen_emit_vector_call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_type, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_callee;
(void)slim_v_arguments;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_destination_type;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_new_call = {0};
slim_v_new_call = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"vec.new", (int64_t)(sizeof("vec.new") - 1)), slim_allocation_region);
(void)slim_v_new_call;
if (slim_v_new_call) {
{
int64_t slim_v_inner_type = {0};
slim_v_inner_type = slim_i64_add(slim_v_destination_type, INT64_C(2));
(void)slim_v_inner_type;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_emit_destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = slim_vec_new(sizeof(", (int64_t)(sizeof(" = slim_vec_new(sizeof(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_emit_type(slim_v_source, slim_v_tokens, slim_v_inner_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"), slim_allocation_region);\n", (int64_t)(sizeof("), slim_allocation_region);\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = true;
}
}
}
}
}
}
else {
{
bool slim_v_len_call = {0};
slim_v_len_call = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"vec.len", (int64_t)(sizeof("vec.len") - 1)), slim_allocation_region);
(void)slim_v_len_call;
if (slim_v_len_call) {
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_emit_destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = (", (int64_t)(sizeof(" = (") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_vector = {0};
slim_v_vector = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_vector;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)").len;\n", (int64_t)(sizeof(").len;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = true;
}
}
}
}
}
else {
{
bool slim_v_get_call = {0};
slim_v_get_call = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"vec.get", (int64_t)(sizeof("vec.get") - 1)), slim_allocation_region);
(void)slim_v_get_call;
if (slim_v_get_call) {
{
int64_t slim_v_index = {0};
slim_v_index = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_arguments, slim_allocation_region);
(void)slim_v_index;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_emit_destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_cast_prefix = {0};
slim_v_cast_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = ((", (int64_t)(sizeof(" = ((") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_cast_prefix;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_emit_type(slim_v_source, slim_v_tokens, slim_v_destination_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_cast_suffix = {0};
slim_v_cast_suffix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" *)(", (int64_t)(sizeof(" *)(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_cast_suffix;
{
SlimUnit slim_v_vector_data = {0};
slim_v_vector_data = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_vector_data;
{
SlimUnit slim_v_check_prefix = {0};
slim_v_check_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)").data)[slim_vec_check_index(&(", (int64_t)(sizeof(").data)[slim_vec_check_index(&(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_check_prefix;
{
SlimUnit slim_v_vector_check = {0};
slim_v_vector_check = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_vector_check;
{
SlimUnit slim_v_comma = {0};
slim_v_comma = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"), ", (int64_t)(sizeof("), ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_comma;
{
SlimUnit slim_v_index_value = {0};
slim_v_index_value = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_index_value;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)")];\n", (int64_t)(sizeof(")];\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = true;
}
}
}
}
}
}
}
}
}
}
}
}
else {
{
bool slim_v_push_call = {0};
slim_v_push_call = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"vec.push", (int64_t)(sizeof("vec.push") - 1)), slim_allocation_region);
(void)slim_v_push_call;
if (slim_v_push_call) {
{
int64_t slim_v_value = {0};
slim_v_value = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_arguments, slim_allocation_region);
(void)slim_v_value;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"if (!slim_vec_push(", (int64_t)(sizeof("if (!slim_vec_push(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_address = {0};
slim_v_address = slim_fn_codegen_emit_binding_address(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_address;
{
bool slim_v_number = {0};
slim_v_number = slim_fn_syntax_first_byte_is_number(slim_v_source, slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_number;
{
SlimUnit slim_v_middle = {0};
if (slim_v_number) {
slim_v_middle = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)", &(int64_t){", (int64_t)(sizeof(", &(int64_t){") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_middle = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)", &(", (int64_t)(sizeof(", &(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_middle;
{
SlimUnit slim_v_pushed_value = {0};
slim_v_pushed_value = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_pushed_value;
{
SlimUnit slim_v_close = {0};
if (slim_v_number) {
slim_v_close = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"})) goto slim_allocation_failed; ", (int64_t)(sizeof("})) goto slim_allocation_failed; ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_close = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"))) goto slim_allocation_failed; ", (int64_t)(sizeof("))) goto slim_allocation_failed; ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_close;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_emit_destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = (SlimUnit){0};\n", (int64_t)(sizeof(" = (SlimUnit){0};\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = true;
}
}
}
}
}
}
}
}
}
}
else {
slim_result = false;
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_user_call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_params;
(void)slim_v_callee;
(void)slim_v_arguments;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_callee_start = {0};
slim_v_callee_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_callee_start;
{
int64_t slim_v_callee_end = {0};
slim_v_callee_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_callee_end;
{
int64_t slim_v_callee_params = {0};
slim_v_callee_params = slim_fn_syntax_find_function_params(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_callee_start, slim_v_callee_end, slim_allocation_region);
(void)slim_v_callee_params;
{
int64_t slim_v_first_callee_param = {0};
slim_v_first_callee_param = slim_i64_add(slim_v_callee_params, INT64_C(1));
(void)slim_v_first_callee_param;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_emit_destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = ", (int64_t)(sizeof(" = ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_emit_function_name(slim_v_source, slim_v_tokens, slim_v_callee, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_open = {0};
slim_v_open = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"(", (int64_t)(sizeof("(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_open;
{
SlimUnit slim_v_args = {0};
slim_v_args = slim_fn_codegen_emit_user_arguments(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_first_callee_param, true, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_args;
{
SlimUnit slim_v_closed = {0};
slim_v_closed = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)");\n", (int64_t)(sizeof(");\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_closed;
{
bool slim_v_allocation_effect = {0};
slim_v_allocation_effect = slim_fn_memory_params_have_alloc_effect(slim_v_source, slim_v_tokens, slim_v_callee_params, slim_allocation_region);
(void)slim_v_allocation_effect;
if (slim_v_allocation_effect) {
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;\n", (int64_t)(sizeof("if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_result = (SlimUnit){0};
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_params, int64_t slim_v_call_form, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_type, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_params;
(void)slim_v_call_form;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_destination_type;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_callee = {0};
slim_v_callee = slim_i64_add(slim_v_call_form, INT64_C(2));
(void)slim_v_callee;
{
int64_t slim_v_arguments = {0};
slim_v_arguments = slim_i64_add(slim_v_call_form, INT64_C(3));
(void)slim_v_arguments;
{
bool slim_v_read_file = {0};
slim_v_read_file = slim_fn_codegen_emit_read_file_call(slim_v_source, slim_v_tokens, slim_v_params, slim_v_callee, slim_v_arguments, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_read_file;
if (slim_v_read_file) {
slim_result = (SlimUnit){0};
}
else {
{
SlimBytes slim_v_runtime = {0};
slim_v_runtime = slim_fn_codegen_runtime_name(slim_v_source, slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_runtime;
{
int64_t slim_v_runtime_length = {0};
slim_v_runtime_length = slim_bytes_len(slim_v_runtime);
(void)slim_v_runtime_length;
{
bool slim_v_has_runtime = {0};
slim_v_has_runtime = slim_v_runtime_length > INT64_C(0);
(void)slim_v_has_runtime;
if (slim_v_has_runtime) {
slim_result = slim_fn_codegen_emit_runtime_assignment(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_destination, slim_v_result, slim_v_runtime, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_comparison = {0};
slim_v_comparison = slim_fn_codegen_emit_comparison(slim_v_source, slim_v_tokens, slim_v_params, slim_v_callee, slim_v_arguments, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_comparison;
if (slim_v_comparison) {
slim_result = (SlimUnit){0};
}
else {
{
bool slim_v_boolean_call = {0};
slim_v_boolean_call = slim_fn_codegen_emit_boolean_call(slim_v_source, slim_v_tokens, slim_v_params, slim_v_callee, slim_v_arguments, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_boolean_call;
if (slim_v_boolean_call) {
slim_result = (SlimUnit){0};
}
else {
{
bool slim_v_conversion = {0};
slim_v_conversion = slim_fn_codegen_emit_u8_conversion(slim_v_source, slim_v_tokens, slim_v_params, slim_v_callee, slim_v_arguments, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_conversion;
if (slim_v_conversion) {
slim_result = (SlimUnit){0};
}
else {
{
bool slim_v_vector_call = {0};
slim_v_vector_call = slim_fn_codegen_emit_collection_call(slim_v_source, slim_v_tokens, slim_v_params, slim_v_callee, slim_v_arguments, slim_v_destination, slim_v_result, slim_v_destination_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_vector_call;
if (slim_v_vector_call) {
slim_result = (SlimUnit){0};
}
else {
slim_result = slim_fn_codegen_emit_user_call(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_params, slim_v_callee, slim_v_arguments, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_codegen_find_record_item(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_name_start;
(void)slim_v_name_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
bool slim_v_record_item = {0};
slim_v_record_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"record", (int64_t)(sizeof("record") - 1)), slim_allocation_region);
(void)slim_v_record_item;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_candidate_start = {0};
slim_v_candidate_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_candidate_start;
{
int64_t slim_v_candidate_end = {0};
slim_v_candidate_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_candidate_end;
{
bool slim_v_same_name = {0};
slim_v_same_name = slim_fn_syntax_spans_equal(slim_v_source, slim_v_candidate_start, slim_v_candidate_end, slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_same_name;
{
bool slim_v_found = {0};
slim_v_found = slim_v_record_item && slim_v_same_name;
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_codegen_find_record_field(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_name_start;
(void)slim_v_name_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_name;
{
int64_t slim_v_candidate_start = {0};
slim_v_candidate_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_candidate_start;
{
int64_t slim_v_candidate_end = {0};
slim_v_candidate_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_candidate_end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_spans_equal(slim_v_source, slim_v_candidate_start, slim_v_candidate_end, slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_make_bindings(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_params, int64_t slim_v_definition, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_params;
(void)slim_v_definition;
(void)slim_v_cursor;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_field = {0};
slim_v_field = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_field;
{
int64_t slim_v_value = {0};
slim_v_value = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_value;
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_field, slim_allocation_region);
(void)slim_v_name_start;
{
int64_t slim_v_name_end = {0};
slim_v_name_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_field, slim_allocation_region);
(void)slim_v_name_end;
{
int64_t slim_v_declared = {0};
slim_v_declared = slim_fn_codegen_find_record_field(slim_v_source, slim_v_tokens, slim_v_definition, slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_declared;
{
int64_t slim_v_type_index = {0};
slim_v_type_index = slim_i64_add(slim_v_declared, INT64_C(2));
(void)slim_v_type_index;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_emit_type(slim_v_source, slim_v_tokens, slim_v_type_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_emit_temp_name(slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_initialized = {0};
slim_v_initialized = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = {0};\n", (int64_t)(sizeof(" = {0};\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_initialized;
{
int64_t slim_v_destination = {0};
slim_v_destination = slim_i64_sub(INT64_C(0), slim_v_cursor);
(void)slim_v_destination;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_emit_expr_full(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_params, slim_v_value, slim_v_destination, false, slim_v_type_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_make_fields(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_cursor;
(void)slim_v_first;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
SlimUnit slim_v_separator = {0};
if (slim_v_first) {
slim_v_separator = (SlimUnit){0};
}
else {
slim_v_separator = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)", ", (int64_t)(sizeof(", ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_separator;
{
int64_t slim_v_field = {0};
slim_v_field = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_field;
{
SlimUnit slim_v_dot = {0};
slim_v_dot = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)".", (int64_t)(sizeof(".") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dot;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_emit_field_name(slim_v_source, slim_v_tokens, slim_v_field, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = ", (int64_t)(sizeof(" = ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_emit_temp_name(slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
slim_v_first = false;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_make(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_expr;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_module_items = {0};
slim_v_module_items = INT64_C(3);
(void)slim_v_module_items;
{
int64_t slim_v_record = {0};
slim_v_record = slim_i64_add(slim_v_expr, INT64_C(2));
(void)slim_v_record;
{
int64_t slim_v_first_field = {0};
slim_v_first_field = slim_i64_add(slim_v_expr, INT64_C(3));
(void)slim_v_first_field;
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_record, slim_allocation_region);
(void)slim_v_name_start;
{
int64_t slim_v_name_end = {0};
slim_v_name_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_record, slim_allocation_region);
(void)slim_v_name_end;
{
int64_t slim_v_item = {0};
slim_v_item = slim_fn_codegen_find_record_item(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_item;
{
int64_t slim_v_fields_form = {0};
slim_v_fields_form = slim_i64_add(slim_v_item, INT64_C(3));
(void)slim_v_fields_form;
{
int64_t slim_v_definition = {0};
slim_v_definition = slim_i64_add(slim_v_fields_form, INT64_C(1));
(void)slim_v_definition;
{
SlimUnit slim_v_open = {0};
slim_v_open = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"{\n", (int64_t)(sizeof("{\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_open;
{
SlimUnit slim_v_bindings = {0};
slim_v_bindings = slim_fn_codegen_emit_make_bindings(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_params, slim_v_definition, slim_v_first_field, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_bindings;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_emit_destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = (", (int64_t)(sizeof(" = (") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_emit_named_type_name(slim_v_source, slim_v_tokens, slim_v_record, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_opened = {0};
slim_v_opened = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"){", (int64_t)(sizeof("){") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_opened;
{
SlimUnit slim_v_fields = {0};
slim_v_fields = slim_fn_codegen_emit_make_fields(slim_v_source, slim_v_tokens, slim_v_params, slim_v_first_field, true, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_fields;
{
SlimUnit slim_v_closed = {0};
slim_v_closed = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"};\n", (int64_t)(sizeof("};\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_closed;
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"}\n", (int64_t)(sizeof("}\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_get(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_expr;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_value = {0};
slim_v_value = slim_i64_add(slim_v_expr, INT64_C(2));
(void)slim_v_value;
{
int64_t slim_v_field = {0};
slim_v_field = slim_i64_add(slim_v_expr, INT64_C(3));
(void)slim_v_field;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_emit_destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = ", (int64_t)(sizeof(" = ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
{
SlimUnit slim_v_dot = {0};
slim_v_dot = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)".", (int64_t)(sizeof(".") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dot;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_emit_field_name(slim_v_source, slim_v_tokens, slim_v_field, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_case_bindings(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_params, int64_t slim_v_cursor, int64_t slim_v_payload_type, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_params;
(void)slim_v_cursor;
(void)slim_v_payload_type;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_emit_type(slim_v_source, slim_v_tokens, slim_v_payload_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_emit_temp_name(slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_initialized = {0};
slim_v_initialized = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = {0};\n", (int64_t)(sizeof(" = {0};\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_initialized;
{
int64_t slim_v_destination = {0};
slim_v_destination = slim_i64_sub(INT64_C(0), slim_v_cursor);
(void)slim_v_destination;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_emit_expr_full(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_params, slim_v_cursor, slim_v_destination, false, slim_v_payload_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_next_type = {0};
slim_v_next_type = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_payload_type, slim_allocation_region);
(void)slim_v_next_type;
slim_v_cursor = slim_v_next;
slim_v_payload_type = slim_v_next_type;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_case_payload(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_cursor, int64_t slim_v_field, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_cursor;
(void)slim_v_field;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
bool slim_v_first = {0};
slim_v_first = slim_v_field == INT64_C(0);
(void)slim_v_first;
{
SlimUnit slim_v_separator = {0};
if (slim_v_first) {
slim_v_separator = (SlimUnit){0};
}
else {
slim_v_separator = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)", ", (int64_t)(sizeof(", ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_separator;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)".slim_", (int64_t)(sizeof(".slim_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_number = {0};
slim_v_number = slim_fn_text_append_nonnegative_i64(slim_v_field, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_number;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = ", (int64_t)(sizeof(" = ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_value = {0};
slim_v_value = slim_fn_codegen_emit_temp_name(slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_value;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_next_field = {0};
slim_v_next_field = slim_i64_add(slim_v_field, INT64_C(1));
(void)slim_v_next_field;
slim_v_cursor = slim_v_next;
slim_v_field = slim_v_next_field;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_case_value(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_expr;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_module_items = {0};
slim_v_module_items = INT64_C(3);
(void)slim_v_module_items;
{
int64_t slim_v_variant = {0};
slim_v_variant = slim_i64_add(slim_v_expr, INT64_C(2));
(void)slim_v_variant;
{
int64_t slim_v_case_name = {0};
slim_v_case_name = slim_i64_add(slim_v_expr, INT64_C(3));
(void)slim_v_case_name;
{
int64_t slim_v_payload = {0};
slim_v_payload = slim_i64_add(slim_v_expr, INT64_C(4));
(void)slim_v_payload;
{
int64_t slim_v_variant_start = {0};
slim_v_variant_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_variant, slim_allocation_region);
(void)slim_v_variant_start;
{
int64_t slim_v_variant_end = {0};
slim_v_variant_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_variant, slim_allocation_region);
(void)slim_v_variant_end;
{
int64_t slim_v_item = {0};
slim_v_item = slim_fn_codegen_find_variant_item(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_variant_start, slim_v_variant_end, slim_allocation_region);
(void)slim_v_item;
{
int64_t slim_v_cases_form = {0};
slim_v_cases_form = slim_i64_add(slim_v_item, INT64_C(3));
(void)slim_v_cases_form;
{
int64_t slim_v_cases = {0};
slim_v_cases = slim_i64_add(slim_v_cases_form, INT64_C(1));
(void)slim_v_cases;
{
int64_t slim_v_case_start = {0};
slim_v_case_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_case_name, slim_allocation_region);
(void)slim_v_case_start;
{
int64_t slim_v_case_end = {0};
slim_v_case_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_case_name, slim_allocation_region);
(void)slim_v_case_end;
{
int64_t slim_v_case_form = {0};
slim_v_case_form = slim_fn_codegen_find_variant_case(slim_v_source, slim_v_tokens, slim_v_cases, slim_v_case_start, slim_v_case_end, slim_allocation_region);
(void)slim_v_case_form;
{
int64_t slim_v_payload_type = {0};
slim_v_payload_type = slim_i64_add(slim_v_case_form, INT64_C(2));
(void)slim_v_payload_type;
{
int64_t slim_v_payload_kind = {0};
slim_v_payload_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_payload, slim_allocation_region);
(void)slim_v_payload_kind;
{
bool slim_v_empty = {0};
slim_v_empty = slim_v_payload_kind == INT64_C(1);
(void)slim_v_empty;
{
SlimUnit slim_v_open = {0};
slim_v_open = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"{\n", (int64_t)(sizeof("{\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_open;
{
SlimUnit slim_v_bindings = {0};
slim_v_bindings = slim_fn_codegen_emit_case_bindings(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_params, slim_v_payload, slim_v_payload_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_bindings;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_emit_destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = (", (int64_t)(sizeof(" = (") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_emit_named_type_name(slim_v_source, slim_v_tokens, slim_v_variant, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_tag_prefix = {0};
slim_v_tag_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"){.tag = ", (int64_t)(sizeof("){.tag = ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tag_prefix;
{
SlimUnit slim_v_tag = {0};
slim_v_tag = slim_fn_codegen_emit_tag_name(slim_v_source, slim_v_tokens, slim_v_variant, slim_v_case_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tag;
{
SlimUnit slim_v_data = {0};
if (slim_v_empty) {
slim_v_data = (SlimUnit){0};
}
else {
{
SlimUnit slim_v_data_prefix = {0};
slim_v_data_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)", .data.", (int64_t)(sizeof(", .data.") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_data_prefix;
{
SlimUnit slim_v_case_field = {0};
slim_v_case_field = slim_fn_codegen_emit_case_name(slim_v_source, slim_v_tokens, slim_v_case_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_case_field;
{
SlimUnit slim_v_payload_open = {0};
slim_v_payload_open = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = {", (int64_t)(sizeof(" = {") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_payload_open;
{
SlimUnit slim_v_payload_values = {0};
slim_v_payload_values = slim_fn_codegen_emit_case_payload(slim_v_source, slim_v_tokens, slim_v_params, slim_v_payload, INT64_C(0), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_payload_values;
slim_v_data = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"}", (int64_t)(sizeof("}") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
(void)slim_v_data;
{
SlimUnit slim_v_closed = {0};
slim_v_closed = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"};\n", (int64_t)(sizeof("};\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_closed;
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"}\n", (int64_t)(sizeof("}\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_codegen_find_parameter_type(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_name_start;
(void)slim_v_name_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"inout", (int64_t)(sizeof("inout") - 1)), slim_allocation_region);
(void)slim_v_borrowed;
{
int64_t slim_v_name = {0};
if (slim_v_borrowed) {
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
}
else {
slim_v_name = slim_v_head;
}
(void)slim_v_name;
{
int64_t slim_v_type_index = {0};
if (slim_v_borrowed) {
slim_v_type_index = slim_i64_add(slim_v_cursor, INT64_C(3));
}
else {
slim_v_type_index = slim_i64_add(slim_v_cursor, INT64_C(2));
}
(void)slim_v_type_index;
{
int64_t slim_v_candidate_start = {0};
slim_v_candidate_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_candidate_start;
{
int64_t slim_v_candidate_end = {0};
slim_v_candidate_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_candidate_end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_spans_equal(slim_v_source, slim_v_candidate_start, slim_v_candidate_end, slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_type_index;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_codegen_find_variant_item(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_name_start;
(void)slim_v_name_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
bool slim_v_variant_item = {0};
slim_v_variant_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"variant", (int64_t)(sizeof("variant") - 1)), slim_allocation_region);
(void)slim_v_variant_item;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_candidate_start = {0};
slim_v_candidate_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_candidate_start;
{
int64_t slim_v_candidate_end = {0};
slim_v_candidate_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_candidate_end;
{
bool slim_v_same_name = {0};
slim_v_same_name = slim_fn_syntax_spans_equal(slim_v_source, slim_v_candidate_start, slim_v_candidate_end, slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_same_name;
{
bool slim_v_found = {0};
slim_v_found = slim_v_variant_item && slim_v_same_name;
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_codegen_find_variant_case(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_name_start;
(void)slim_v_name_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_name;
{
int64_t slim_v_candidate_start = {0};
slim_v_candidate_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_candidate_start;
{
int64_t slim_v_candidate_end = {0};
slim_v_candidate_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_candidate_end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_spans_equal(slim_v_source, slim_v_candidate_start, slim_v_candidate_end, slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_variant_bindings(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_value, int64_t slim_v_case_name, int64_t slim_v_bindings, int64_t slim_v_payload_type, int64_t slim_v_field, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_value;
(void)slim_v_case_name;
(void)slim_v_bindings;
(void)slim_v_payload_type;
(void)slim_v_field;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_binding = {0};
slim_v_binding = slim_i64_add(slim_v_bindings, INT64_C(1));
(void)slim_v_binding;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_binding, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_emit_type(slim_v_source, slim_v_tokens, slim_v_payload_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_emit_source_name(slim_v_source, slim_v_tokens, slim_v_binding, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = ", (int64_t)(sizeof(" = ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_scrutinee = {0};
slim_v_scrutinee = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, INT64_C(0), slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_scrutinee;
{
SlimUnit slim_v_data = {0};
slim_v_data = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)".data.", (int64_t)(sizeof(".data.") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_data;
{
SlimUnit slim_v_case_field = {0};
slim_v_case_field = slim_fn_codegen_emit_case_name(slim_v_source, slim_v_tokens, slim_v_case_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_case_field;
{
SlimUnit slim_v_payload_field = {0};
slim_v_payload_field = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)".slim_", (int64_t)(sizeof(".slim_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_payload_field;
{
SlimUnit slim_v_number = {0};
slim_v_number = slim_fn_text_append_nonnegative_i64(slim_v_field, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_number;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
{
int64_t slim_v_next_bindings = {0};
slim_v_next_bindings = slim_i64_add(slim_v_bindings, INT64_C(1));
(void)slim_v_next_bindings;
{
int64_t slim_v_next_type = {0};
slim_v_next_type = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_payload_type, slim_allocation_region);
(void)slim_v_next_type;
{
int64_t slim_v_next_field = {0};
slim_v_next_field = slim_i64_add(slim_v_field, INT64_C(1));
(void)slim_v_next_field;
slim_v_bindings = slim_v_next_bindings;
slim_v_payload_type = slim_v_next_type;
slim_v_field = slim_v_next_field;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_variant_arms(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_params, int64_t slim_v_value, int64_t slim_v_variant_name, int64_t slim_v_variant_cases, int64_t slim_v_cursor, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_type, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_params;
(void)slim_v_value;
(void)slim_v_variant_name;
(void)slim_v_variant_cases;
(void)slim_v_cursor;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_destination_type;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_case_name = {0};
slim_v_case_name = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_case_name;
{
int64_t slim_v_case_start = {0};
slim_v_case_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_case_name, slim_allocation_region);
(void)slim_v_case_start;
{
int64_t slim_v_case_end = {0};
slim_v_case_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_case_name, slim_allocation_region);
(void)slim_v_case_end;
{
int64_t slim_v_case_form = {0};
slim_v_case_form = slim_fn_codegen_find_variant_case(slim_v_source, slim_v_tokens, slim_v_variant_cases, slim_v_case_start, slim_v_case_end, slim_allocation_region);
(void)slim_v_case_form;
{
int64_t slim_v_payload_type = {0};
slim_v_payload_type = slim_i64_add(slim_v_case_form, INT64_C(2));
(void)slim_v_payload_type;
{
int64_t slim_v_payload_kind = {0};
slim_v_payload_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_payload_type, slim_allocation_region);
(void)slim_v_payload_kind;
{
bool slim_v_empty = {0};
slim_v_empty = slim_v_payload_kind == INT64_C(1);
(void)slim_v_empty;
{
int64_t slim_v_bindings = {0};
slim_v_bindings = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_bindings;
{
int64_t slim_v_body = {0};
if (slim_v_empty) {
slim_v_body = slim_v_bindings;
}
else {
slim_v_body = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_bindings, slim_allocation_region);
}
(void)slim_v_body;
{
SlimUnit slim_v_case_prefix = {0};
slim_v_case_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"case ", (int64_t)(sizeof("case ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_case_prefix;
{
SlimUnit slim_v_tag = {0};
slim_v_tag = slim_fn_codegen_emit_tag_name(slim_v_source, slim_v_tokens, slim_v_variant_name, slim_v_case_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tag;
{
SlimUnit slim_v_opened = {0};
slim_v_opened = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)": {\n", (int64_t)(sizeof(": {\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_opened;
{
SlimUnit slim_v_bound = {0};
if (slim_v_empty) {
slim_v_bound = (SlimUnit){0};
}
else {
slim_v_bound = slim_fn_codegen_emit_variant_bindings(slim_v_source, slim_v_tokens, slim_v_value, slim_v_case_name, slim_v_bindings, slim_v_payload_type, INT64_C(0), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_bound;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_emit_expr_full(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_params, slim_v_body, slim_v_destination, slim_v_result, slim_v_destination_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
{
SlimUnit slim_v_closed = {0};
slim_v_closed = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"break;\n}\n", (int64_t)(sizeof("break;\n}\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_closed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_boolean_match(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_params, int64_t slim_v_value, int64_t slim_v_first_arm, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_type, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_params;
(void)slim_v_value;
(void)slim_v_first_arm;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_destination_type;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_first_pattern = {0};
slim_v_first_pattern = slim_i64_add(slim_v_first_arm, INT64_C(1));
(void)slim_v_first_pattern;
{
int64_t slim_v_first_body = {0};
slim_v_first_body = slim_i64_add(slim_v_first_arm, INT64_C(2));
(void)slim_v_first_body;
{
int64_t slim_v_second_arm = {0};
slim_v_second_arm = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_first_arm, slim_allocation_region);
(void)slim_v_second_arm;
{
int64_t slim_v_second_body = {0};
slim_v_second_body = slim_i64_add(slim_v_second_arm, INT64_C(2));
(void)slim_v_second_body;
{
bool slim_v_first_true = {0};
slim_v_first_true = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_first_pattern, slim_bytes_static((const uint8_t *)"true", (int64_t)(sizeof("true") - 1)), slim_allocation_region);
(void)slim_v_first_true;
{
SlimUnit slim_v_if_open = {0};
slim_v_if_open = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"if (", (int64_t)(sizeof("if (") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_if_open;
{
SlimUnit slim_v_negate = {0};
if (slim_v_first_true) {
slim_v_negate = (SlimUnit){0};
}
else {
slim_v_negate = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"!", (int64_t)(sizeof("!") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_negate;
{
SlimUnit slim_v_condition = {0};
slim_v_condition = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_condition;
{
SlimUnit slim_v_branch_open = {0};
slim_v_branch_open = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)") {\n", (int64_t)(sizeof(") {\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_branch_open;
{
SlimUnit slim_v_first_emitted = {0};
slim_v_first_emitted = slim_fn_codegen_emit_expr_full(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_params, slim_v_first_body, slim_v_destination, slim_v_result, slim_v_destination_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_first_emitted;
{
SlimUnit slim_v_branch_middle = {0};
slim_v_branch_middle = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"}\nelse {\n", (int64_t)(sizeof("}\nelse {\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_branch_middle;
{
SlimUnit slim_v_second_emitted = {0};
slim_v_second_emitted = slim_fn_codegen_emit_expr_full(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_params, slim_v_second_body, slim_v_destination, slim_v_result, slim_v_destination_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_second_emitted;
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"}\n", (int64_t)(sizeof("}\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_variant_match(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_params, int64_t slim_v_value, int64_t slim_v_first_arm, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_type, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_params;
(void)slim_v_value;
(void)slim_v_first_arm;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_destination_type;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_name_start;
{
int64_t slim_v_name_end = {0};
slim_v_name_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_name_end;
{
int64_t slim_v_first_param = {0};
slim_v_first_param = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_first_param;
{
int64_t slim_v_variant_type = {0};
slim_v_variant_type = slim_fn_codegen_find_parameter_type(slim_v_source, slim_v_tokens, slim_v_first_param, slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_variant_type;
{
int64_t slim_v_variant_start = {0};
slim_v_variant_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_variant_type, slim_allocation_region);
(void)slim_v_variant_start;
{
int64_t slim_v_variant_end = {0};
slim_v_variant_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_variant_type, slim_allocation_region);
(void)slim_v_variant_end;
{
int64_t slim_v_variant_item = {0};
slim_v_variant_item = slim_fn_codegen_find_variant_item(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_variant_start, slim_v_variant_end, slim_allocation_region);
(void)slim_v_variant_item;
{
int64_t slim_v_cases = {0};
slim_v_cases = slim_i64_add(slim_v_variant_item, INT64_C(3));
(void)slim_v_cases;
{
int64_t slim_v_variant_cases = {0};
slim_v_variant_cases = slim_i64_add(slim_v_cases, INT64_C(1));
(void)slim_v_variant_cases;
{
SlimUnit slim_v_switch_open = {0};
slim_v_switch_open = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"switch (", (int64_t)(sizeof("switch (") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_switch_open;
{
SlimUnit slim_v_scrutinee = {0};
slim_v_scrutinee = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_scrutinee;
{
SlimUnit slim_v_tag = {0};
slim_v_tag = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)".tag) {\n", (int64_t)(sizeof(".tag) {\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tag;
{
SlimUnit slim_v_arms = {0};
slim_v_arms = slim_fn_codegen_emit_variant_arms(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_params, slim_v_value, slim_v_variant_type, slim_v_variant_cases, slim_v_first_arm, slim_v_destination, slim_v_result, slim_v_destination_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_arms;
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"default: slim_rt_trap(\"invalid variant tag\");\n}\n", (int64_t)(sizeof("default: slim_rt_trap(\"invalid variant tag\");\n}\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_atom_assignment(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_value, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_value;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_emit_destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = ", (int64_t)(sizeof(" = ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_function(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_item, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_item;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_item, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_item, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_type = {0};
slim_v_return_type = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_type;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_return_type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_effects, slim_allocation_region);
(void)slim_v_body;
{
int64_t slim_v_body_end = {0};
slim_v_body_end = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_end;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"static ", (int64_t)(sizeof("static ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_returned = {0};
slim_v_returned = slim_fn_codegen_emit_type(slim_v_source, slim_v_tokens, slim_v_return_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_returned;
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_emit_function_name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_open = {0};
slim_v_open = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"(", (int64_t)(sizeof("(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_open;
{
int64_t slim_v_first_param = {0};
slim_v_first_param = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_first_param;
{
SlimUnit slim_v_parameters = {0};
slim_v_parameters = slim_fn_codegen_emit_parameters(slim_v_source, slim_v_tokens, slim_v_first_param, true, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_parameters;
{
SlimUnit slim_v_block = {0};
slim_v_block = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)") {\n", (int64_t)(sizeof(") {\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_block;
{
SlimUnit slim_v_voids = {0};
slim_v_voids = slim_fn_codegen_emit_parameter_voids(slim_v_source, slim_v_tokens, slim_v_first_param, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_voids;
{
bool slim_v_local_region = {0};
slim_v_local_region = slim_fn_memory_function_uses_local_region(slim_v_source, slim_v_tokens, slim_v_item, slim_allocation_region);
(void)slim_v_local_region;
{
bool slim_v_allocation_effect = {0};
slim_v_allocation_effect = slim_fn_memory_function_has_alloc_effect(slim_v_source, slim_v_tokens, slim_v_item, slim_allocation_region);
(void)slim_v_allocation_effect;
{
SlimUnit slim_v_region = {0};
if (slim_v_local_region) {
slim_v_region = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"SlimRegion slim_function_region;\nslim_region_init(&slim_function_region, slim_region);\nSlimRegion *slim_allocation_region = &slim_function_region;\n(void)slim_allocation_region;\n", (int64_t)(sizeof("SlimRegion slim_function_region;\nslim_region_init(&slim_function_region, slim_region);\nSlimRegion *slim_allocation_region = &slim_function_region;\n(void)slim_allocation_region;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_region = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"SlimRegion *slim_allocation_region = slim_region;\n(void)slim_allocation_region;\n", (int64_t)(sizeof("SlimRegion *slim_allocation_region = slim_region;\n(void)slim_allocation_region;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_region;
{
SlimUnit slim_v_result_type = {0};
slim_v_result_type = slim_fn_codegen_emit_type(slim_v_source, slim_v_tokens, slim_v_return_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_result_type;
{
SlimUnit slim_v_result_decl = {0};
slim_v_result_decl = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" slim_result = {0};\n", (int64_t)(sizeof(" slim_result = {0};\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_result_decl;
{
SlimUnit slim_v_initial_failure = {0};
if (slim_v_allocation_effect) {
slim_v_initial_failure = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;\n", (int64_t)(sizeof("if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_initial_failure = (SlimUnit){0};
}
(void)slim_v_initial_failure;
{
bool slim_v_recursive = {0};
slim_v_recursive = slim_fn_codegen_contains_atom(slim_v_source, slim_v_tokens, slim_v_body, slim_v_body_end, slim_bytes_static((const uint8_t *)"recur", (int64_t)(sizeof("recur") - 1)), slim_allocation_region);
(void)slim_v_recursive;
{
SlimUnit slim_v_label = {0};
if (slim_v_recursive) {
slim_v_label = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_recur: ;\n", (int64_t)(sizeof("slim_recur: ;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_label = (SlimUnit){0};
}
(void)slim_v_label;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_emit_expr_full(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_params, slim_v_body, INT64_C(0), true, slim_v_return_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
{
SlimUnit slim_v_failure_label = {0};
if (slim_v_allocation_effect) {
slim_v_failure_label = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_allocation_failed: ;\n", (int64_t)(sizeof("slim_allocation_failed: ;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_failure_label = (SlimUnit){0};
}
(void)slim_v_failure_label;
{
SlimUnit slim_v_destroyed = {0};
if (slim_v_local_region) {
slim_v_destroyed = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_region_destroy(&slim_function_region);\n", (int64_t)(sizeof("slim_region_destroy(&slim_function_region);\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_destroyed = (SlimUnit){0};
}
(void)slim_v_destroyed;
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"return slim_result;\n}\n\n", (int64_t)(sizeof("return slim_result;\n}\n\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_functions(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_cursor;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
bool slim_v_function_item = {0};
slim_v_function_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_item;
{
SlimUnit slim_v_emitted = {0};
if (slim_v_function_item) {
slim_v_emitted = slim_fn_codegen_emit_function(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_emitted = (SlimUnit){0};
}
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_emit_program(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimUnit slim_v_header = {0};
slim_v_header = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"/* generated deterministically by self-hosted slimc */\n#include \"slim_rt.h\"\n#include <string.h>\n\n", (int64_t)(sizeof("/* generated deterministically by self-hosted slimc */\n#include \"slim_rt.h\"\n#include <string.h>\n\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_header;
{
int64_t slim_v_module_items = {0};
slim_v_module_items = INT64_C(3);
(void)slim_v_module_items;
{
SlimUnit slim_v_forwards = {0};
slim_v_forwards = slim_fn_codegen_emit_forward_types(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_forwards;
{
SlimUnit slim_v_data_items = {0};
slim_v_data_items = slim_fn_codegen_emit_data_items(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_data_items;
{
SlimUnit slim_v_prototypes = {0};
slim_v_prototypes = slim_fn_codegen_emit_prototypes(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prototypes;
{
SlimUnit slim_v_functions = {0};
slim_v_functions = slim_fn_codegen_emit_functions(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_module_items, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_functions;
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"int main(int argc, char **argv) {\nSlimAllocStatus slim_status;\nslim_alloc_status_init(&slim_status);\nSlimRegion slim_root;\nslim_rt_init(&slim_root, &slim_status);\nSlimVec slim_args = slim_vec_new(sizeof(SlimBytes), &slim_root);\nfor (int slim_i = 0; slim_i < argc; ++slim_i) {\nSlimBytes slim_arg = slim_bytes_static((const uint8_t *)argv[slim_i], (int64_t)strlen(argv[slim_i]));\nif (!slim_vec_push(&slim_args, &slim_arg)) {\nslim_alloc_report(&slim_status);\nslim_rt_shutdown();\nreturn 71;\n}\n}\nint64_t slim_exit_code = slim_fn_main(slim_args, &slim_root);\nif (slim_region_failed(&slim_root)) {\nslim_alloc_report(&slim_status);\nslim_rt_shutdown();\nreturn 71;\n}\nif (slim_exit_code < 0 || slim_exit_code > 255) {\nslim_rt_trap(\"main result is outside 0..255\");\n}\nslim_rt_shutdown();\nreturn (int)slim_exit_code;\n}\n", (int64_t)(sizeof("int main(int argc, char **argv) {\nSlimAllocStatus slim_status;\nslim_alloc_status_init(&slim_status);\nSlimRegion slim_root;\nslim_rt_init(&slim_root, &slim_status);\nSlimVec slim_args = slim_vec_new(sizeof(SlimBytes), &slim_root);\nfor (int slim_i = 0; slim_i < argc; ++slim_i) {\nSlimBytes slim_arg = slim_bytes_static((const uint8_t *)argv[slim_i], (int64_t)strlen(argv[slim_i]));\nif (!slim_vec_push(&slim_args, &slim_arg)) {\nslim_alloc_report(&slim_status);\nslim_rt_shutdown();\nreturn 71;\n}\n}\nint64_t slim_exit_code = slim_fn_main(slim_args, &slim_root);\nif (slim_region_failed(&slim_root)) {\nslim_alloc_report(&slim_status);\nslim_rt_shutdown();\nreturn 71;\n}\nif (slim_exit_code < 0 || slim_exit_code > 255) {\nslim_rt_trap(\"main result is outside 0..255\");\n}\nslim_rt_shutdown();\nreturn (int)slim_exit_code;\n}\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_codegen_emit_read_file_call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_callee;
(void)slim_v_arguments;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_read_file = {0};
slim_v_read_file = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.read-file", (int64_t)(sizeof("io.read-file") - 1)), slim_allocation_region);
(void)slim_v_read_file;
if (!slim_v_read_file) {
slim_result = false;
}
else {
{
int64_t slim_v_output_argument = {0};
slim_v_output_argument = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_arguments, slim_allocation_region);
(void)slim_v_output_argument;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_emit_destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" = slim_read_file(", (int64_t)(sizeof(" = slim_read_file(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_path_value = {0};
slim_v_path_value = slim_fn_codegen_emit_value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_path_value;
{
SlimUnit slim_v_comma = {0};
slim_v_comma = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)", ", (int64_t)(sizeof(", ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_comma;
{
SlimUnit slim_v_output_address = {0};
slim_v_output_address = slim_fn_codegen_emit_binding_address(slim_v_source, slim_v_tokens, slim_v_params, slim_v_output_argument, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_output_address;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)");\nif (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;\n", (int64_t)(sizeof(");\nif (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = true;
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_compiler_check_input_path(SlimBytes slim_v_path, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_project = {0};
slim_v_project = slim_fn_project_path_is_project(slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_project;
if (slim_v_project) {
slim_result = slim_fn_project_check_project_path(slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_result = slim_fn_check_check_path_all(slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static SlimUnit slim_fn_compiler_format_tokens(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, int64_t slim_v_length, int64_t slim_v_previous_kind, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_v_previous_kind;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"\n", (int64_t)(sizeof("\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_is_right = {0};
slim_v_is_right = slim_v_kind == INT64_C(1);
(void)slim_v_is_right;
{
bool slim_v_previous_left = {0};
slim_v_previous_left = slim_v_previous_kind == INT64_C(0);
(void)slim_v_previous_left;
{
bool slim_v_noninitial = {0};
slim_v_noninitial = slim_v_index > INT64_C(0);
(void)slim_v_noninitial;
{
bool slim_v_not_right = {0};
slim_v_not_right = !slim_v_is_right;
(void)slim_v_not_right;
{
bool slim_v_not_previous_left = {0};
slim_v_not_previous_left = !slim_v_previous_left;
(void)slim_v_not_previous_left;
{
bool slim_v_after_form = {0};
slim_v_after_form = slim_v_noninitial && slim_v_not_previous_left;
(void)slim_v_after_form;
{
bool slim_v_separated = {0};
slim_v_separated = slim_v_after_form && slim_v_not_right;
(void)slim_v_separated;
{
SlimUnit slim_v_separator = {0};
if (slim_v_separated) {
slim_v_separator = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_separator = (SlimUnit){0};
}
(void)slim_v_separator;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_text_append_token(slim_v_source, slim_v_tokens, slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
slim_v_previous_kind = slim_v_kind;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_compiler_format_path(SlimBytes slim_v_path, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_input_bytes = {0};
slim_v_input_bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_input_bytes;
{
bool slim_v_input_read = {0};
slim_v_input_read = slim_read_file(slim_v_path, &slim_v_input_bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_input_read;
{
SlimBytes slim_v_input = {0};
slim_v_input = slim_bytes_freeze(slim_v_input_bytes);
(void)slim_v_input;
{
SlimVec slim_v_tokens = {0};
slim_v_tokens = slim_vec_new(sizeof(Slim_type_syntax_Token), slim_allocation_region);
(void)slim_v_tokens;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_lex(slim_v_input, INT64_C(0), &slim_v_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
int64_t slim_v_length = {0};
slim_v_length = (slim_v_tokens).len;
(void)slim_v_length;
{
SlimUnit slim_v_formatted = {0};
slim_v_formatted = slim_fn_compiler_format_tokens(slim_v_input, &slim_v_tokens, INT64_C(0), slim_v_length, INT64_C(0), &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_formatted;
{
SlimBytes slim_v_bytes = {0};
slim_v_bytes = slim_bytes_freeze(slim_v_output);
(void)slim_v_bytes;
{
SlimUnit slim_v_shown = {0};
slim_v_shown = slim_print_bytes(slim_v_bytes);
(void)slim_v_shown;
slim_result = INT64_C(0);
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_compiler_compile_path(SlimBytes slim_v_path, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_project = {0};
slim_v_project = slim_fn_project_path_is_project(slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_project;
if (slim_v_project) {
slim_result = slim_fn_project_compile_project_path(slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
SlimVec slim_v_input_bytes = {0};
slim_v_input_bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_input_bytes;
{
bool slim_v_input_read = {0};
slim_v_input_read = slim_read_file(slim_v_path, &slim_v_input_bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_input_read;
{
SlimBytes slim_v_input = {0};
slim_v_input = slim_bytes_freeze(slim_v_input_bytes);
(void)slim_v_input;
{
SlimVec slim_v_tokens = {0};
slim_v_tokens = slim_vec_new(sizeof(Slim_type_syntax_Token), slim_allocation_region);
(void)slim_v_tokens;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_lex(slim_v_input, INT64_C(0), &slim_v_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_emit_program(slim_v_input, &slim_v_tokens, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
{
SlimBytes slim_v_generated = {0};
slim_v_generated = slim_bytes_freeze(slim_v_output);
(void)slim_v_generated;
{
SlimUnit slim_v_shown = {0};
slim_v_shown = slim_print_bytes(slim_v_generated);
(void)slim_v_shown;
slim_result = INT64_C(0);
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_compiler_run_compile(SlimVec * slim_v_args, int64_t slim_v_arg_count, SlimBytes slim_v_path, SlimRegion *slim_region) {
(void)slim_v_args;
(void)slim_v_arg_count;
(void)slim_v_path;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_project = {0};
slim_v_project = slim_fn_project_path_is_project(slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_project;
if (!slim_v_project) {
slim_result = slim_fn_compiler_compile_path(slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_enough = {0};
slim_v_enough = slim_v_arg_count >= INT64_C(4);
(void)slim_v_enough;
{
bool slim_v_scheduled = {0};
if (!slim_v_enough) {
slim_v_scheduled = false;
}
else {
{
SlimBytes slim_v_option = {0};
slim_v_option = ((SlimBytes *)((*slim_v_args)).data)[slim_vec_check_index(&((*slim_v_args)), INT64_C(2))];
(void)slim_v_option;
{
int64_t slim_v_option_length = {0};
slim_v_option_length = slim_bytes_len(slim_v_option);
(void)slim_v_option_length;
slim_v_scheduled = slim_fn_syntax_span_equal(slim_v_option, INT64_C(0), slim_v_option_length, slim_bytes_static((const uint8_t *)"--jobs", (int64_t)(sizeof("--jobs") - 1)), slim_allocation_region);
}
}
}
(void)slim_v_scheduled;
{
int64_t slim_v_jobs = {0};
if (!slim_v_scheduled) {
slim_v_jobs = INT64_C(1);
}
else {
{
SlimBytes slim_v_requested = {0};
slim_v_requested = ((SlimBytes *)((*slim_v_args)).data)[slim_vec_check_index(&((*slim_v_args)), INT64_C(3))];
(void)slim_v_requested;
slim_v_jobs = slim_fn_scheduler_parse_jobs(slim_v_requested, slim_allocation_region);
}
}
(void)slim_v_jobs;
slim_result = slim_fn_project_compile_project_path_jobs(slim_v_path, slim_v_jobs, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_compiler_run_cache(SlimVec * slim_v_args, int64_t slim_v_arg_count, SlimRegion *slim_region) {
(void)slim_v_args;
(void)slim_v_arg_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_has_paths = {0};
slim_v_has_paths = slim_v_arg_count >= INT64_C(4);
(void)slim_v_has_paths;
if (!slim_v_has_paths) {
{
SlimUnit slim_v_usage = {0};
slim_v_usage = slim_println(slim_bytes_static((const uint8_t *)"usage: slimc cache MANIFEST CACHE", (int64_t)(sizeof("usage: slimc cache MANIFEST CACHE") - 1)));
(void)slim_v_usage;
slim_result = INT64_C(64);
}
}
else {
{
SlimBytes slim_v_manifest_path = {0};
slim_v_manifest_path = ((SlimBytes *)((*slim_v_args)).data)[slim_vec_check_index(&((*slim_v_args)), INT64_C(2))];
(void)slim_v_manifest_path;
{
SlimBytes slim_v_cache_path = {0};
slim_v_cache_path = ((SlimBytes *)((*slim_v_args)).data)[slim_vec_check_index(&((*slim_v_args)), INT64_C(3))];
(void)slim_v_cache_path;
slim_result = slim_fn_cache_run(slim_v_manifest_path, slim_v_cache_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_compiler_run_session(SlimVec * slim_v_args, int64_t slim_v_arg_count, SlimRegion *slim_region) {
(void)slim_v_args;
(void)slim_v_arg_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_has_paths = {0};
slim_v_has_paths = slim_v_arg_count >= INT64_C(4);
(void)slim_v_has_paths;
if (!slim_v_has_paths) {
{
SlimUnit slim_v_usage = {0};
slim_v_usage = slim_println(slim_bytes_static((const uint8_t *)"usage: slimc session INITIAL UPDATED [RECOVERED]", (int64_t)(sizeof("usage: slimc session INITIAL UPDATED [RECOVERED]") - 1)));
(void)slim_v_usage;
slim_result = INT64_C(64);
}
}
else {
{
SlimBytes slim_v_initial_path = {0};
slim_v_initial_path = ((SlimBytes *)((*slim_v_args)).data)[slim_vec_check_index(&((*slim_v_args)), INT64_C(2))];
(void)slim_v_initial_path;
{
SlimBytes slim_v_updated_path = {0};
slim_v_updated_path = ((SlimBytes *)((*slim_v_args)).data)[slim_vec_check_index(&((*slim_v_args)), INT64_C(3))];
(void)slim_v_updated_path;
{
bool slim_v_recovering = {0};
slim_v_recovering = slim_v_arg_count >= INT64_C(5);
(void)slim_v_recovering;
if (!slim_v_recovering) {
slim_result = slim_fn_session_run(slim_v_initial_path, slim_v_updated_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
SlimBytes slim_v_recovered_path = {0};
slim_v_recovered_path = ((SlimBytes *)((*slim_v_args)).data)[slim_vec_check_index(&((*slim_v_args)), INT64_C(4))];
(void)slim_v_recovered_path;
slim_result = slim_fn_session_run_recovery(slim_v_initial_path, slim_v_updated_path, slim_v_recovered_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_compiler_run(SlimVec slim_v_args, SlimRegion *slim_region) {
(void)slim_v_args;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_arg_count = {0};
slim_v_arg_count = (slim_v_args).len;
(void)slim_v_arg_count;
{
bool slim_v_enough = {0};
slim_v_enough = slim_v_arg_count >= INT64_C(2);
(void)slim_v_enough;
if (!slim_v_enough) {
{
SlimUnit slim_v_usage = {0};
slim_v_usage = slim_println(slim_bytes_static((const uint8_t *)"usage: slimc SOURCE | slimc check SOURCE | slimc fmt SOURCE", (int64_t)(sizeof("usage: slimc SOURCE | slimc check SOURCE | slimc fmt SOURCE") - 1)));
(void)slim_v_usage;
slim_result = INT64_C(64);
}
}
else {
{
SlimBytes slim_v_command = {0};
slim_v_command = ((SlimBytes *)(slim_v_args).data)[slim_vec_check_index(&(slim_v_args), INT64_C(1))];
(void)slim_v_command;
{
int64_t slim_v_command_length = {0};
slim_v_command_length = slim_bytes_len(slim_v_command);
(void)slim_v_command_length;
{
bool slim_v_formatting = {0};
slim_v_formatting = slim_fn_syntax_span_equal(slim_v_command, INT64_C(0), slim_v_command_length, slim_bytes_static((const uint8_t *)"fmt", (int64_t)(sizeof("fmt") - 1)), slim_allocation_region);
(void)slim_v_formatting;
if (slim_v_formatting) {
{
bool slim_v_has_path = {0};
slim_v_has_path = slim_v_arg_count >= INT64_C(3);
(void)slim_v_has_path;
if (slim_v_has_path) {
{
SlimBytes slim_v_path = {0};
slim_v_path = ((SlimBytes *)(slim_v_args).data)[slim_vec_check_index(&(slim_v_args), INT64_C(2))];
(void)slim_v_path;
slim_result = slim_fn_compiler_format_path(slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
else {
{
SlimUnit slim_v_usage = {0};
slim_v_usage = slim_println(slim_bytes_static((const uint8_t *)"usage: slimc fmt SOURCE", (int64_t)(sizeof("usage: slimc fmt SOURCE") - 1)));
(void)slim_v_usage;
slim_result = INT64_C(64);
}
}
}
}
else {
{
bool slim_v_checking = {0};
slim_v_checking = slim_fn_syntax_span_equal(slim_v_command, INT64_C(0), slim_v_command_length, slim_bytes_static((const uint8_t *)"check", (int64_t)(sizeof("check") - 1)), slim_allocation_region);
(void)slim_v_checking;
if (slim_v_checking) {
{
bool slim_v_has_source = {0};
slim_v_has_source = slim_v_arg_count >= INT64_C(3);
(void)slim_v_has_source;
if (slim_v_has_source) {
{
SlimBytes slim_v_source_path = {0};
slim_v_source_path = ((SlimBytes *)(slim_v_args).data)[slim_vec_check_index(&(slim_v_args), INT64_C(2))];
(void)slim_v_source_path;
slim_result = slim_fn_compiler_check_input_path(slim_v_source_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
else {
{
SlimUnit slim_v_usage = {0};
slim_v_usage = slim_println(slim_bytes_static((const uint8_t *)"usage: slimc check SOURCE", (int64_t)(sizeof("usage: slimc check SOURCE") - 1)));
(void)slim_v_usage;
slim_result = INT64_C(64);
}
}
}
}
else {
{
bool slim_v_interface_command = {0};
slim_v_interface_command = slim_fn_syntax_span_equal(slim_v_command, INT64_C(0), slim_v_command_length, slim_bytes_static((const uint8_t *)"interfaces", (int64_t)(sizeof("interfaces") - 1)), slim_allocation_region);
(void)slim_v_interface_command;
if (slim_v_interface_command) {
{
bool slim_v_has_manifest = {0};
slim_v_has_manifest = slim_v_arg_count >= INT64_C(3);
(void)slim_v_has_manifest;
if (slim_v_has_manifest) {
{
SlimBytes slim_v_manifest_path = {0};
slim_v_manifest_path = ((SlimBytes *)(slim_v_args).data)[slim_vec_check_index(&(slim_v_args), INT64_C(2))];
(void)slim_v_manifest_path;
slim_result = slim_fn_project_interfaces_path(slim_v_manifest_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
else {
{
SlimUnit slim_v_usage = {0};
slim_v_usage = slim_println(slim_bytes_static((const uint8_t *)"usage: slimc interfaces MANIFEST", (int64_t)(sizeof("usage: slimc interfaces MANIFEST") - 1)));
(void)slim_v_usage;
slim_result = INT64_C(64);
}
}
}
}
else {
{
bool slim_v_cache_command = {0};
slim_v_cache_command = slim_fn_syntax_span_equal(slim_v_command, INT64_C(0), slim_v_command_length, slim_bytes_static((const uint8_t *)"cache", (int64_t)(sizeof("cache") - 1)), slim_allocation_region);
(void)slim_v_cache_command;
if (slim_v_cache_command) {
slim_result = slim_fn_compiler_run_cache(&slim_v_args, slim_v_arg_count, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_session_command = {0};
slim_v_session_command = slim_fn_syntax_span_equal(slim_v_command, INT64_C(0), slim_v_command_length, slim_bytes_static((const uint8_t *)"session", (int64_t)(sizeof("session") - 1)), slim_allocation_region);
(void)slim_v_session_command;
if (slim_v_session_command) {
slim_result = slim_fn_compiler_run_session(&slim_v_args, slim_v_arg_count, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_result = slim_fn_compiler_run_compile(&slim_v_args, slim_v_arg_count, slim_v_command, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_main(SlimVec slim_v_args, SlimRegion *slim_region) {
(void)slim_v_args;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_result = slim_fn_compiler_run(slim_v_args, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_memory_storage_join(int64_t slim_v_left, int64_t slim_v_right, SlimRegion *slim_region) {
(void)slim_v_left;
(void)slim_v_right;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_left_greater = {0};
slim_v_left_greater = slim_v_left > slim_v_right;
(void)slim_v_left_greater;
if (slim_v_left_greater) {
slim_result = slim_v_left;
}
else {
slim_result = slim_v_right;
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_memory_find_type_item(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_name_start;
(void)slim_v_name_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
bool slim_v_record_item = {0};
slim_v_record_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"record", (int64_t)(sizeof("record") - 1)), slim_allocation_region);
(void)slim_v_record_item;
{
bool slim_v_variant_item = {0};
slim_v_variant_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"variant", (int64_t)(sizeof("variant") - 1)), slim_allocation_region);
(void)slim_v_variant_item;
{
bool slim_v_type_item = {0};
slim_v_type_item = slim_v_record_item || slim_v_variant_item;
(void)slim_v_type_item;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_candidate_start = {0};
slim_v_candidate_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_candidate_start;
{
int64_t slim_v_candidate_end = {0};
slim_v_candidate_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_candidate_end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_spans_equal(slim_v_source, slim_v_candidate_start, slim_v_candidate_end, slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_same;
{
bool slim_v_found = {0};
slim_v_found = slim_v_type_item && slim_v_same;
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_memory_record_storage(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_cursor, int64_t slim_v_depth, int64_t slim_v_current, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_cursor;
(void)slim_v_depth;
(void)slim_v_current;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_current;
}
else {
{
int64_t slim_v_type_index = {0};
slim_v_type_index = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_type_index;
{
int64_t slim_v_storage = {0};
slim_v_storage = slim_fn_memory_type_storage_kind_from(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_type_index, slim_v_depth, slim_allocation_region);
(void)slim_v_storage;
{
int64_t slim_v_joined = {0};
slim_v_joined = slim_fn_memory_storage_join(slim_v_current, slim_v_storage, slim_allocation_region);
(void)slim_v_joined;
{
bool slim_v_owned = {0};
slim_v_owned = slim_v_joined == INT64_C(2);
(void)slim_v_owned;
if (slim_v_owned) {
slim_result = INT64_C(2);
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
slim_v_current = slim_v_joined;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_memory_payload_storage(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_cursor, int64_t slim_v_depth, int64_t slim_v_current, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_cursor;
(void)slim_v_depth;
(void)slim_v_current;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_current;
}
else {
{
int64_t slim_v_storage = {0};
slim_v_storage = slim_fn_memory_type_storage_kind_from(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_cursor, slim_v_depth, slim_allocation_region);
(void)slim_v_storage;
{
int64_t slim_v_joined = {0};
slim_v_joined = slim_fn_memory_storage_join(slim_v_current, slim_v_storage, slim_allocation_region);
(void)slim_v_joined;
{
bool slim_v_owned = {0};
slim_v_owned = slim_v_joined == INT64_C(2);
(void)slim_v_owned;
if (slim_v_owned) {
slim_result = INT64_C(2);
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
slim_v_current = slim_v_joined;
goto slim_recur;
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_memory_variant_storage(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_cursor, int64_t slim_v_depth, int64_t slim_v_current, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_cursor;
(void)slim_v_depth;
(void)slim_v_current;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_current;
}
else {
{
int64_t slim_v_payload = {0};
slim_v_payload = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_payload;
{
int64_t slim_v_storage = {0};
slim_v_storage = slim_fn_memory_payload_storage(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_payload, slim_v_depth, slim_v_current, slim_allocation_region);
(void)slim_v_storage;
{
bool slim_v_owned = {0};
slim_v_owned = slim_v_storage == INT64_C(2);
(void)slim_v_owned;
if (slim_v_owned) {
slim_result = INT64_C(2);
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
slim_v_current = slim_v_storage;
goto slim_recur;
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_memory_type_storage_kind_from(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_items, int64_t slim_v_type_index, int64_t slim_v_depth, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_items;
(void)slim_v_type_index;
(void)slim_v_depth;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_too_deep = {0};
slim_v_too_deep = slim_v_depth >= INT64_C(64);
(void)slim_v_too_deep;
if (slim_v_too_deep) {
slim_result = INT64_C(2);
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_type_index, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_composite = {0};
slim_v_composite = slim_v_kind == INT64_C(0);
(void)slim_v_composite;
if (slim_v_composite) {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_type_index, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_id_type = {0};
slim_v_id_type = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"Id", (int64_t)(sizeof("Id") - 1)), slim_allocation_region);
(void)slim_v_id_type;
if (slim_v_id_type) {
slim_result = INT64_C(0);
}
else {
slim_result = INT64_C(2);
}
}
}
}
else {
{
int64_t slim_v_scalar_kind = {0};
slim_v_scalar_kind = slim_fn_syntax_scalar_type_kind(slim_v_source, slim_v_tokens, slim_v_type_index, slim_allocation_region);
(void)slim_v_scalar_kind;
{
bool slim_v_scalar = {0};
slim_v_scalar = slim_v_scalar_kind >= INT64_C(0);
(void)slim_v_scalar;
if (slim_v_scalar) {
{
bool slim_v_bytes_type = {0};
slim_v_bytes_type = slim_v_scalar_kind == INT64_C(4);
(void)slim_v_bytes_type;
if (slim_v_bytes_type) {
slim_result = INT64_C(1);
}
else {
slim_result = INT64_C(0);
}
}
}
else {
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_type_index, slim_allocation_region);
(void)slim_v_name_start;
{
int64_t slim_v_name_end = {0};
slim_v_name_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_type_index, slim_allocation_region);
(void)slim_v_name_end;
{
int64_t slim_v_item = {0};
slim_v_item = slim_fn_memory_find_type_item(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_item;
{
bool slim_v_found = {0};
slim_v_found = slim_v_item >= INT64_C(0);
(void)slim_v_found;
if (!slim_v_found) {
slim_result = INT64_C(2);
}
else {
{
int64_t slim_v_next_depth = {0};
slim_v_next_depth = slim_i64_add(slim_v_depth, INT64_C(1));
(void)slim_v_next_depth;
{
int64_t slim_v_body = {0};
slim_v_body = slim_i64_add(slim_v_item, INT64_C(3));
(void)slim_v_body;
{
int64_t slim_v_first = {0};
slim_v_first = slim_i64_add(slim_v_body, INT64_C(1));
(void)slim_v_first;
{
bool slim_v_record_item = {0};
slim_v_record_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_item, slim_bytes_static((const uint8_t *)"record", (int64_t)(sizeof("record") - 1)), slim_allocation_region);
(void)slim_v_record_item;
if (slim_v_record_item) {
slim_result = slim_fn_memory_record_storage(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_first, slim_v_next_depth, INT64_C(0), slim_allocation_region);
}
else {
slim_result = slim_fn_memory_variant_storage(slim_v_source, slim_v_tokens, slim_v_module_items, slim_v_first, slim_v_next_depth, INT64_C(0), slim_allocation_region);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_memory_type_storage_kind(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_type_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_type_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_result = slim_fn_memory_type_storage_kind_from(slim_v_source, slim_v_tokens, INT64_C(3), slim_v_type_index, INT64_C(0), slim_allocation_region);
return slim_result;
}

static bool slim_fn_memory_parameters_have_inout(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"inout", (int64_t)(sizeof("inout") - 1)), slim_allocation_region);
(void)slim_v_borrowed;
if (slim_v_borrowed) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_memory_function_uses_local_region(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_function, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_function;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_function, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_type = {0};
slim_v_return_type = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_type;
{
int64_t slim_v_return_storage = {0};
slim_v_return_storage = slim_fn_memory_type_storage_kind(slim_v_source, slim_v_tokens, slim_v_return_type, slim_allocation_region);
(void)slim_v_return_storage;
{
bool slim_v_no_return_storage = {0};
slim_v_no_return_storage = slim_v_return_storage == INT64_C(0);
(void)slim_v_no_return_storage;
{
int64_t slim_v_first_parameter = {0};
slim_v_first_parameter = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_first_parameter;
{
bool slim_v_output = {0};
slim_v_output = slim_fn_memory_parameters_have_inout(slim_v_source, slim_v_tokens, slim_v_first_parameter, slim_allocation_region);
(void)slim_v_output;
{
bool slim_v_no_output = {0};
slim_v_no_output = !slim_v_output;
(void)slim_v_no_output;
slim_result = slim_v_no_return_storage && slim_v_no_output;
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_memory_effect_list_has_alloc(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
bool slim_v_allocation = {0};
slim_v_allocation = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"alloc", (int64_t)(sizeof("alloc") - 1)), slim_allocation_region);
(void)slim_v_allocation;
if (slim_v_allocation) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_memory_params_have_alloc_effect(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_return_type = {0};
slim_v_return_type = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_type;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_return_type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_first = {0};
slim_v_first = slim_i64_add(slim_v_effects, INT64_C(2));
(void)slim_v_first;
slim_result = slim_fn_memory_effect_list_has_alloc(slim_v_source, slim_v_tokens, slim_v_first, slim_allocation_region);
}
}
}
return slim_result;
}

static bool slim_fn_memory_function_has_alloc_effect(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_function, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_function;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_function, INT64_C(3));
(void)slim_v_params;
slim_result = slim_fn_memory_params_have_alloc_effect(slim_v_source, slim_v_tokens, slim_v_params, slim_allocation_region);
}
return slim_result;
}

static int64_t slim_fn_memory_last_named_use(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_name_start, int64_t slim_v_name_end, int64_t slim_v_last, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_v_name_start;
(void)slim_v_name_end;
(void)slim_v_last;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_last;
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_named = {0};
slim_v_named = slim_v_kind == INT64_C(2);
(void)slim_v_named;
{
int64_t slim_v_candidate_start = {0};
slim_v_candidate_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_candidate_start;
{
int64_t slim_v_candidate_end = {0};
slim_v_candidate_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_candidate_end;
{
bool slim_v_same = {0};
if (slim_v_named) {
slim_v_same = slim_fn_syntax_spans_equal(slim_v_source, slim_v_candidate_start, slim_v_candidate_end, slim_v_name_start, slim_v_name_end, slim_allocation_region);
}
else {
slim_v_same = false;
}
(void)slim_v_same;
{
int64_t slim_v_next_last = {0};
if (slim_v_same) {
slim_v_next_last = slim_v_candidate_end;
}
else {
slim_v_next_last = slim_v_last;
}
(void)slim_v_next_last;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
slim_v_last = slim_v_next_last;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_memory_span_has_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_v_name_start;
(void)slim_v_name_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_found = {0};
slim_v_found = slim_fn_memory_last_named_use(slim_v_source, slim_v_tokens, slim_v_cursor, slim_v_end, slim_v_name_start, slim_v_name_end, INT64_C(-1), slim_allocation_region);
(void)slim_v_found;
slim_result = slim_v_found >= INT64_C(0);
}
return slim_result;
}

static int64_t slim_fn_memory_result_expression(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_expr;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_expr, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
if (!slim_v_form) {
slim_result = slim_v_expr;
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_expr, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_let_form = {0};
slim_v_let_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"let", (int64_t)(sizeof("let") - 1)), slim_allocation_region);
(void)slim_v_let_form;
{
bool slim_v_set_form = {0};
slim_v_set_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"set", (int64_t)(sizeof("set") - 1)), slim_allocation_region);
(void)slim_v_set_form;
{
bool slim_v_pair_form = {0};
slim_v_pair_form = slim_v_let_form || slim_v_set_form;
(void)slim_v_pair_form;
if (!slim_v_pair_form) {
slim_result = slim_v_expr;
}
else {
{
int64_t slim_v_offset = {0};
if (slim_v_let_form) {
slim_v_offset = INT64_C(3);
}
else {
slim_v_offset = INT64_C(2);
}
(void)slim_v_offset;
{
int64_t slim_v_first = {0};
slim_v_first = slim_i64_add(slim_v_expr, slim_v_offset);
(void)slim_v_first;
{
int64_t slim_v_value = {0};
if (slim_v_let_form) {
slim_v_value = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_first, slim_allocation_region);
}
else {
slim_v_value = slim_v_first;
}
(void)slim_v_value;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_body;
slim_v_expr = slim_v_body;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_memory_escapes_to_inout(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_v_name_start;
(void)slim_v_name_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
if (!slim_v_form) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_call_form = {0};
slim_v_call_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"call", (int64_t)(sizeof("call") - 1)), slim_allocation_region);
(void)slim_v_call_form;
if (!slim_v_call_form) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
else {
{
int64_t slim_v_callee = {0};
slim_v_callee = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_callee;
{
bool slim_v_vector_push = {0};
slim_v_vector_push = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"vec.push", (int64_t)(sizeof("vec.push") - 1)), slim_allocation_region);
(void)slim_v_vector_push;
{
bool slim_v_arena_add = {0};
slim_v_arena_add = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"arena.add", (int64_t)(sizeof("arena.add") - 1)), slim_allocation_region);
(void)slim_v_arena_add;
{
bool slim_v_store = {0};
slim_v_store = slim_v_vector_push || slim_v_arena_add;
(void)slim_v_store;
if (!slim_v_store) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
else {
{
int64_t slim_v_target = {0};
slim_v_target = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_target;
{
int64_t slim_v_value = {0};
slim_v_value = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_target, slim_allocation_region);
(void)slim_v_value;
{
int64_t slim_v_value_start = {0};
slim_v_value_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_value_start;
{
int64_t slim_v_value_end = {0};
slim_v_value_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_value_end;
{
bool slim_v_same_value = {0};
slim_v_same_value = slim_fn_syntax_spans_equal(slim_v_source, slim_v_value_start, slim_v_value_end, slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_same_value;
{
bool slim_v_target_inout = {0};
if (slim_v_same_value) {
slim_v_target_inout = slim_fn_syntax_name_is_inout(slim_v_source, slim_v_tokens, slim_v_params, slim_v_target, slim_allocation_region);
}
else {
slim_v_target_inout = false;
}
(void)slim_v_target_inout;
if (slim_v_target_inout) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_memory_escape_kind(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body, int64_t slim_v_body_end, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_body;
(void)slim_v_body_end;
(void)slim_v_name_start;
(void)slim_v_name_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
int64_t slim_v_result = {0};
slim_v_result = slim_fn_memory_result_expression(slim_v_source, slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_result;
{
int64_t slim_v_result_end = {0};
slim_v_result_end = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_result, slim_allocation_region);
(void)slim_v_result_end;
{
bool slim_v_result_escape = {0};
slim_v_result_escape = slim_fn_memory_span_has_name(slim_v_source, slim_v_tokens, slim_v_result, slim_v_result_end, slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_result_escape;
{
bool slim_v_output_escape = {0};
slim_v_output_escape = slim_fn_memory_escapes_to_inout(slim_v_source, slim_v_tokens, slim_v_params, slim_v_body, slim_v_body_end, slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_output_escape;
if (slim_v_result_escape) {
if (slim_v_output_escape) {
slim_result = INT64_C(3);
}
else {
slim_result = INT64_C(1);
}
}
else {
if (slim_v_output_escape) {
slim_result = INT64_C(2);
}
else {
slim_result = INT64_C(0);
}
}
}
}
}
}
return slim_result;
}

static SlimUnit slim_fn_memory_append_value_plan(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body, int64_t slim_v_body_end, int64_t slim_v_name, int64_t slim_v_type_index, bool slim_v_borrowed, int64_t slim_v_declared, SlimVec * slim_v_values, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_body;
(void)slim_v_body_end;
(void)slim_v_name;
(void)slim_v_type_index;
(void)slim_v_borrowed;
(void)slim_v_declared;
(void)slim_v_values;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_start;
{
int64_t slim_v_name_end = {0};
slim_v_name_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_end;
{
int64_t slim_v_storage = {0};
slim_v_storage = slim_fn_memory_type_storage_kind(slim_v_source, slim_v_tokens, slim_v_type_index, slim_allocation_region);
(void)slim_v_storage;
{
Slim_type_memory_ValuePlan slim_v_plan = {0};
{
int64_t slim_t_30446 = {0};
slim_t_30446 = slim_v_name;
int64_t slim_t_30450 = {0};
slim_t_30450 = slim_v_type_index;
int64_t slim_t_30454 = {0};
slim_t_30454 = slim_v_storage;
bool slim_t_30458 = {0};
slim_t_30458 = slim_v_borrowed;
int64_t slim_t_30462 = {0};
slim_t_30462 = slim_v_declared;
int64_t slim_t_30466 = {0};
slim_t_30466 = slim_v_name_end;
int64_t slim_t_30470 = {0};
slim_t_30470 = INT64_C(0);
slim_v_plan = (Slim_type_memory_ValuePlan){.slim_field_name = slim_t_30446, .slim_field_type_index = slim_t_30450, .slim_field_storage = slim_t_30454, .slim_field_borrowed = slim_t_30458, .slim_field_declared = slim_t_30462, .slim_field_last_use = slim_t_30466, .slim_field_escape = slim_t_30470};
}
(void)slim_v_plan;
if (!slim_vec_push(slim_v_values, &(slim_v_plan))) goto slim_allocation_failed; slim_result = (SlimUnit){0};
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_memory_append_parameter_plans(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body, int64_t slim_v_body_end, int64_t slim_v_cursor, SlimVec * slim_v_values, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_body;
(void)slim_v_body_end;
(void)slim_v_cursor;
(void)slim_v_values;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"inout", (int64_t)(sizeof("inout") - 1)), slim_allocation_region);
(void)slim_v_borrowed;
{
int64_t slim_v_name = {0};
if (slim_v_borrowed) {
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
}
else {
slim_v_name = slim_v_head;
}
(void)slim_v_name;
{
int64_t slim_v_type_index = {0};
if (slim_v_borrowed) {
slim_v_type_index = slim_i64_add(slim_v_cursor, INT64_C(3));
}
else {
slim_v_type_index = slim_i64_add(slim_v_cursor, INT64_C(2));
}
(void)slim_v_type_index;
{
SlimUnit slim_v_appended = {0};
slim_v_appended = slim_fn_memory_append_value_plan(slim_v_source, slim_v_tokens, slim_v_params, slim_v_body, slim_v_body_end, slim_v_name, slim_v_type_index, slim_v_borrowed, slim_v_cursor, slim_v_values, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_appended;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_memory_append_local_plans(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body, int64_t slim_v_body_end, int64_t slim_v_cursor, SlimVec * slim_v_values, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_body;
(void)slim_v_body_end;
(void)slim_v_cursor;
(void)slim_v_values;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_body_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_let_form = {0};
if (slim_v_form) {
slim_v_let_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"let", (int64_t)(sizeof("let") - 1)), slim_allocation_region);
}
else {
slim_v_let_form = false;
}
(void)slim_v_let_form;
{
SlimUnit slim_v_appended = {0};
if (slim_v_let_form) {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_type_index = {0};
slim_v_type_index = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_type_index;
slim_v_appended = slim_fn_memory_append_value_plan(slim_v_source, slim_v_tokens, slim_v_params, slim_v_body, slim_v_body_end, slim_v_name, slim_v_type_index, false, slim_v_cursor, slim_v_values, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
slim_v_appended = (SlimUnit){0};
}
(void)slim_v_appended;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_memory_append_allocation_plans(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_region, SlimVec * slim_v_allocations, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_v_region;
(void)slim_v_allocations;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
{
bool slim_v_allocation = {0};
if (!slim_v_form) {
slim_v_allocation = false;
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_call_form = {0};
slim_v_call_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"call", (int64_t)(sizeof("call") - 1)), slim_allocation_region);
(void)slim_v_call_form;
if (!slim_v_call_form) {
slim_v_allocation = false;
}
else {
{
int64_t slim_v_callee = {0};
slim_v_callee = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_callee;
{
bool slim_v_vector_new = {0};
slim_v_vector_new = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"vec.new", (int64_t)(sizeof("vec.new") - 1)), slim_allocation_region);
(void)slim_v_vector_new;
{
bool slim_v_arena_new = {0};
slim_v_arena_new = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"arena.new", (int64_t)(sizeof("arena.new") - 1)), slim_allocation_region);
(void)slim_v_arena_new;
slim_v_allocation = slim_v_vector_new || slim_v_arena_new;
}
}
}
}
}
}
}
(void)slim_v_allocation;
{
SlimUnit slim_v_appended = {0};
if (slim_v_allocation) {
{
Slim_type_memory_AllocationPlan slim_v_plan = {0};
{
int64_t slim_t_31071 = {0};
slim_t_31071 = slim_v_cursor;
int64_t slim_t_31075 = {0};
slim_t_31075 = slim_v_region;
slim_v_plan = (Slim_type_memory_AllocationPlan){.slim_field_site = slim_t_31071, .slim_field_region = slim_t_31075};
}
(void)slim_v_plan;
if (!slim_vec_push(slim_v_allocations, &(slim_v_plan))) goto slim_allocation_failed; slim_v_appended = (SlimUnit){0};
}
}
else {
slim_v_appended = (SlimUnit){0};
}
(void)slim_v_appended;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_memory_conservative_escape(int64_t slim_v_return_storage, bool slim_v_output, SlimRegion *slim_region) {
(void)slim_v_return_storage;
(void)slim_v_output;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_returns = {0};
slim_v_returns = slim_v_return_storage > INT64_C(0);
(void)slim_v_returns;
if (slim_v_returns) {
if (slim_v_output) {
slim_result = INT64_C(3);
}
else {
slim_result = INT64_C(1);
}
}
else {
if (slim_v_output) {
slim_result = INT64_C(2);
}
else {
slim_result = INT64_C(0);
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static SlimUnit slim_fn_memory_finalize_value_plans(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body, int64_t slim_v_body_end, int64_t slim_v_return_storage, bool slim_v_output, SlimVec * slim_v_values, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_body;
(void)slim_v_body_end;
(void)slim_v_return_storage;
(void)slim_v_output;
(void)slim_v_values;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_values)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
Slim_type_memory_ValuePlan slim_v_value = {0};
slim_v_value = ((Slim_type_memory_ValuePlan *)((*slim_v_values)).data)[slim_vec_check_index(&((*slim_v_values)), slim_v_index)];
(void)slim_v_value;
{
int64_t slim_v_name = {0};
slim_v_name = slim_v_value.slim_field_name;
(void)slim_v_name;
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_start;
{
int64_t slim_v_name_end = {0};
slim_v_name_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_end;
{
bool slim_v_bounded = {0};
slim_v_bounded = slim_v_index < INT64_C(64);
(void)slim_v_bounded;
{
int64_t slim_v_last_use = {0};
if (slim_v_bounded) {
{
int64_t slim_v_declared = {0};
slim_v_declared = slim_v_value.slim_field_declared;
(void)slim_v_declared;
slim_v_last_use = slim_fn_memory_last_named_use(slim_v_source, slim_v_tokens, slim_v_declared, slim_v_body_end, slim_v_name_start, slim_v_name_end, slim_v_name_end, slim_allocation_region);
}
}
else {
slim_v_last_use = slim_v_body_end;
}
(void)slim_v_last_use;
{
int64_t slim_v_escape = {0};
if (slim_v_bounded) {
slim_v_escape = slim_fn_memory_escape_kind(slim_v_source, slim_v_tokens, slim_v_params, slim_v_body, slim_v_body_end, slim_v_name_start, slim_v_name_end, slim_allocation_region);
}
else {
{
int64_t slim_v_storage = {0};
slim_v_storage = slim_v_value.slim_field_storage;
(void)slim_v_storage;
{
bool slim_v_stored = {0};
slim_v_stored = slim_v_storage > INT64_C(0);
(void)slim_v_stored;
if (slim_v_stored) {
slim_v_escape = slim_fn_memory_conservative_escape(slim_v_return_storage, slim_v_output, slim_allocation_region);
}
else {
slim_v_escape = INT64_C(0);
}
}
}
}
(void)slim_v_escape;
{
Slim_type_memory_ValuePlan slim_v_updated = {0};
{
int64_t slim_t_31422 = {0};
slim_t_31422 = slim_v_name;
int64_t slim_t_31426 = {0};
slim_t_31426 = slim_v_value.slim_field_type_index;
int64_t slim_t_31434 = {0};
slim_t_31434 = slim_v_value.slim_field_storage;
bool slim_t_31442 = {0};
slim_t_31442 = slim_v_value.slim_field_borrowed;
int64_t slim_t_31450 = {0};
slim_t_31450 = slim_v_value.slim_field_declared;
int64_t slim_t_31458 = {0};
slim_t_31458 = slim_v_last_use;
int64_t slim_t_31462 = {0};
slim_t_31462 = slim_v_escape;
slim_v_updated = (Slim_type_memory_ValuePlan){.slim_field_name = slim_t_31422, .slim_field_type_index = slim_t_31426, .slim_field_storage = slim_t_31434, .slim_field_borrowed = slim_t_31442, .slim_field_declared = slim_t_31450, .slim_field_last_use = slim_t_31458, .slim_field_escape = slim_t_31462};
}
(void)slim_v_updated;
{
SlimUnit slim_v_replaced = {0};
slim_vec_set(slim_v_values, slim_v_index, &slim_v_updated); slim_v_replaced = (SlimUnit){0};
(void)slim_v_replaced;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_memory_count_escaping_values(SlimVec * slim_v_values, int64_t slim_v_index, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_values;
(void)slim_v_index;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_values)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
Slim_type_memory_ValuePlan slim_v_value = {0};
slim_v_value = ((Slim_type_memory_ValuePlan *)((*slim_v_values)).data)[slim_vec_check_index(&((*slim_v_values)), slim_v_index)];
(void)slim_v_value;
{
int64_t slim_v_storage = {0};
slim_v_storage = slim_v_value.slim_field_storage;
(void)slim_v_storage;
{
int64_t slim_v_escape = {0};
slim_v_escape = slim_v_value.slim_field_escape;
(void)slim_v_escape;
{
bool slim_v_stored = {0};
slim_v_stored = slim_v_storage > INT64_C(0);
(void)slim_v_stored;
{
bool slim_v_escapes = {0};
slim_v_escapes = slim_v_escape > INT64_C(0);
(void)slim_v_escapes;
{
bool slim_v_escaping = {0};
slim_v_escaping = slim_v_stored && slim_v_escapes;
(void)slim_v_escaping;
{
int64_t slim_v_increment = {0};
if (slim_v_escaping) {
slim_v_increment = INT64_C(1);
}
else {
slim_v_increment = INT64_C(0);
}
(void)slim_v_increment;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_increment);
(void)slim_v_total;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
slim_v_count = slim_v_total;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static Slim_type_memory_FunctionPlan slim_fn_memory_build_function_plan(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_function, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_function;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
Slim_type_memory_FunctionPlan slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_function, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_type = {0};
slim_v_return_type = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_type;
{
int64_t slim_v_return_storage = {0};
slim_v_return_storage = slim_fn_memory_type_storage_kind(slim_v_source, slim_v_tokens, slim_v_return_type, slim_allocation_region);
(void)slim_v_return_storage;
{
bool slim_v_local_region = {0};
slim_v_local_region = slim_fn_memory_function_uses_local_region(slim_v_source, slim_v_tokens, slim_v_function, slim_allocation_region);
(void)slim_v_local_region;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_return_type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_effects, slim_allocation_region);
(void)slim_v_body;
{
int64_t slim_v_body_end = {0};
slim_v_body_end = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_end;
{
SlimVec slim_v_values = {0};
slim_v_values = slim_vec_new(sizeof(Slim_type_memory_ValuePlan), slim_allocation_region);
(void)slim_v_values;
{
int64_t slim_v_first_parameter = {0};
slim_v_first_parameter = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_first_parameter;
{
SlimUnit slim_v_parameters = {0};
slim_v_parameters = slim_fn_memory_append_parameter_plans(slim_v_source, slim_v_tokens, slim_v_params, slim_v_body, slim_v_body_end, slim_v_first_parameter, &slim_v_values, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_parameters;
{
SlimUnit slim_v_locals = {0};
slim_v_locals = slim_fn_memory_append_local_plans(slim_v_source, slim_v_tokens, slim_v_params, slim_v_body, slim_v_body_end, slim_v_body, &slim_v_values, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_locals;
{
int64_t slim_v_value_count = {0};
slim_v_value_count = (slim_v_values).len;
(void)slim_v_value_count;
{
bool slim_v_liveness_precise = {0};
slim_v_liveness_precise = slim_v_value_count <= INT64_C(64);
(void)slim_v_liveness_precise;
{
int64_t slim_v_first_parameter_for_output = {0};
slim_v_first_parameter_for_output = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_first_parameter_for_output;
{
bool slim_v_output_escape = {0};
slim_v_output_escape = slim_fn_memory_parameters_have_inout(slim_v_source, slim_v_tokens, slim_v_first_parameter_for_output, slim_allocation_region);
(void)slim_v_output_escape;
{
SlimUnit slim_v_finalized = {0};
slim_v_finalized = slim_fn_memory_finalize_value_plans(slim_v_source, slim_v_tokens, slim_v_params, slim_v_body, slim_v_body_end, slim_v_return_storage, slim_v_output_escape, &slim_v_values, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_finalized;
{
SlimVec slim_v_allocations = {0};
slim_v_allocations = slim_vec_new(sizeof(Slim_type_memory_AllocationPlan), slim_allocation_region);
(void)slim_v_allocations;
{
int64_t slim_v_region = {0};
if (slim_v_local_region) {
slim_v_region = INT64_C(0);
}
else {
slim_v_region = INT64_C(1);
}
(void)slim_v_region;
{
SlimUnit slim_v_allocation_plans = {0};
slim_v_allocation_plans = slim_fn_memory_append_allocation_plans(slim_v_source, slim_v_tokens, slim_v_body, slim_v_body_end, slim_v_region, &slim_v_allocations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_allocation_plans;
{
SlimVec slim_v_destructions = {0};
slim_v_destructions = slim_vec_new(sizeof(Slim_type_memory_DestructionPlan), slim_allocation_region);
(void)slim_v_destructions;
{
SlimUnit slim_v_destruction = {0};
if (slim_v_local_region) {
{
Slim_type_memory_DestructionPlan slim_v_plan = {0};
{
int64_t slim_t_31958 = {0};
slim_t_31958 = INT64_C(0);
int64_t slim_t_31962 = {0};
slim_t_31962 = slim_v_body_end;
bool slim_t_31966 = {0};
slim_t_31966 = true;
slim_v_plan = (Slim_type_memory_DestructionPlan){.slim_field_region = slim_t_31958, .slim_field_end = slim_t_31962, .slim_field_reverse = slim_t_31966};
}
(void)slim_v_plan;
if (!slim_vec_push(&slim_v_destructions, &(slim_v_plan))) goto slim_allocation_failed; slim_v_destruction = (SlimUnit){0};
}
}
else {
slim_v_destruction = (SlimUnit){0};
}
(void)slim_v_destruction;
{
int64_t slim_v_escaping = {0};
slim_v_escaping = slim_fn_memory_count_escaping_values(&slim_v_values, INT64_C(0), INT64_C(0), slim_allocation_region);
(void)slim_v_escaping;
{
int64_t slim_t_31998 = {0};
slim_t_31998 = slim_v_function;
int64_t slim_t_32002 = {0};
slim_t_32002 = slim_v_return_storage;
bool slim_t_32006 = {0};
slim_t_32006 = slim_v_local_region;
bool slim_t_32010 = {0};
slim_t_32010 = slim_v_liveness_precise;
SlimVec slim_t_32014 = {0};
slim_t_32014 = slim_v_values;
SlimVec slim_t_32018 = {0};
slim_t_32018 = slim_v_allocations;
SlimVec slim_t_32022 = {0};
slim_t_32022 = slim_v_destructions;
int64_t slim_t_32026 = {0};
slim_t_32026 = slim_v_escaping;
slim_result = (Slim_type_memory_FunctionPlan){.slim_field_function = slim_t_31998, .slim_field_return_storage = slim_t_32002, .slim_field_local_region = slim_t_32006, .slim_field_liveness_precise = slim_t_32010, .slim_field_values = slim_t_32014, .slim_field_allocations = slim_t_32018, .slim_field_destructions = slim_t_32022, .slim_field_escaping_values = slim_t_32026};
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_memory_append_function_plans(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimVec * slim_v_functions, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_v_index;
(void)slim_v_functions;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_declarations)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
Slim_type_ir_Declaration slim_v_declaration = {0};
slim_v_declaration = ((Slim_type_ir_Declaration *)((*slim_v_declarations)).data)[slim_vec_check_index(&((*slim_v_declarations)), slim_v_index)];
(void)slim_v_declaration;
{
int64_t slim_v_form = {0};
slim_v_form = slim_v_declaration.slim_field_form;
(void)slim_v_form;
{
bool slim_v_function_item = {0};
slim_v_function_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_form, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_item;
{
SlimUnit slim_v_appended = {0};
if (slim_v_function_item) {
{
Slim_type_memory_FunctionPlan slim_v_plan = {0};
slim_v_plan = slim_fn_memory_build_function_plan(slim_v_source, slim_v_tokens, slim_v_form, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_plan;
if (!slim_vec_push(slim_v_functions, &(slim_v_plan))) goto slim_allocation_failed; slim_v_appended = (SlimUnit){0};
}
}
else {
slim_v_appended = (SlimUnit){0};
}
(void)slim_v_appended;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static Slim_type_memory_Plan slim_fn_memory_analyze(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
Slim_type_memory_Plan slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_functions = {0};
slim_v_functions = slim_vec_new(sizeof(Slim_type_memory_FunctionPlan), slim_allocation_region);
(void)slim_v_functions;
{
SlimUnit slim_v_appended = {0};
slim_v_appended = slim_fn_memory_append_function_plans(slim_v_source, slim_v_tokens, slim_v_declarations, INT64_C(0), &slim_v_functions, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_appended;
{
SlimVec slim_t_32275 = {0};
slim_t_32275 = slim_v_functions;
slim_result = (Slim_type_memory_Plan){.slim_field_functions = slim_t_32275};
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_project_cross_span_chars_equal(SlimBytes slim_v_left, int64_t slim_v_left_start, SlimBytes slim_v_right, int64_t slim_v_right_start, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region) {
(void)slim_v_left;
(void)slim_v_left_start;
(void)slim_v_right;
(void)slim_v_right_start;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index == slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = true;
}
else {
{
int64_t slim_v_left_index = {0};
slim_v_left_index = slim_i64_add(slim_v_left_start, slim_v_index);
(void)slim_v_left_index;
{
int64_t slim_v_right_index = {0};
slim_v_right_index = slim_i64_add(slim_v_right_start, slim_v_index);
(void)slim_v_right_index;
{
uint8_t slim_v_left_byte = {0};
slim_v_left_byte = slim_bytes_get(slim_v_left, slim_v_left_index);
(void)slim_v_left_byte;
{
uint8_t slim_v_right_byte = {0};
slim_v_right_byte = slim_bytes_get(slim_v_right, slim_v_right_index);
(void)slim_v_right_byte;
{
int64_t slim_v_left_code = {0};
slim_v_left_code = (int64_t)slim_v_left_byte;
(void)slim_v_left_code;
{
int64_t slim_v_right_code = {0};
slim_v_right_code = (int64_t)slim_v_right_byte;
(void)slim_v_right_code;
{
bool slim_v_same = {0};
slim_v_same = slim_v_left_code == slim_v_right_code;
(void)slim_v_same;
if (slim_v_same) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
else {
slim_result = false;
}
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_project_cross_spans_equal(SlimBytes slim_v_left, int64_t slim_v_left_start, int64_t slim_v_left_end, SlimBytes slim_v_right, int64_t slim_v_right_start, int64_t slim_v_right_end, SlimRegion *slim_region) {
(void)slim_v_left;
(void)slim_v_left_start;
(void)slim_v_left_end;
(void)slim_v_right;
(void)slim_v_right_start;
(void)slim_v_right_end;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_left_length = {0};
slim_v_left_length = slim_i64_sub(slim_v_left_end, slim_v_left_start);
(void)slim_v_left_length;
{
int64_t slim_v_right_length = {0};
slim_v_right_length = slim_i64_sub(slim_v_right_end, slim_v_right_start);
(void)slim_v_right_length;
{
bool slim_v_same_length = {0};
slim_v_same_length = slim_v_left_length == slim_v_right_length;
(void)slim_v_same_length;
if (slim_v_same_length) {
slim_result = slim_fn_project_cross_span_chars_equal(slim_v_left, slim_v_left_start, slim_v_right, slim_v_right_start, INT64_C(0), slim_v_left_length, slim_allocation_region);
}
else {
slim_result = false;
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_project_last_path_slash(SlimBytes slim_v_path, int64_t slim_v_index, int64_t slim_v_length, int64_t slim_v_last, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_v_last;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_last;
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_path, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
bool slim_v_slash = {0};
slim_v_slash = slim_v_code == INT64_C(47);
(void)slim_v_slash;
{
int64_t slim_v_next_last = {0};
if (slim_v_slash) {
slim_v_next_last = slim_i64_add(slim_v_index, INT64_C(1));
}
else {
slim_v_next_last = slim_v_last;
}
(void)slim_v_next_last;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
slim_v_last = slim_v_next_last;
goto slim_recur;
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static SlimBytes slim_fn_project_project_module_path(SlimBytes slim_v_manifest_path, SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_path_token, SlimRegion *slim_region) {
(void)slim_v_manifest_path;
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_path_token;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimBytes slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
int64_t slim_v_manifest_length = {0};
slim_v_manifest_length = slim_bytes_len(slim_v_manifest_path);
(void)slim_v_manifest_length;
{
int64_t slim_v_prefix_end = {0};
slim_v_prefix_end = slim_fn_project_last_path_slash(slim_v_manifest_path, INT64_C(0), slim_v_manifest_length, INT64_C(0), slim_allocation_region);
(void)slim_v_prefix_end;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_span(slim_v_manifest_path, INT64_C(0), slim_v_prefix_end, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
int64_t slim_v_token_start_index = {0};
slim_v_token_start_index = slim_fn_syntax_token_start(slim_v_manifest_tokens, slim_v_path_token, slim_allocation_region);
(void)slim_v_token_start_index;
{
int64_t slim_v_content_start = {0};
slim_v_content_start = slim_i64_add(slim_v_token_start_index, INT64_C(1));
(void)slim_v_content_start;
{
int64_t slim_v_token_end_index = {0};
slim_v_token_end_index = slim_fn_syntax_token_end(slim_v_manifest_tokens, slim_v_path_token, slim_allocation_region);
(void)slim_v_token_end_index;
{
int64_t slim_v_content_end = {0};
slim_v_content_end = slim_i64_sub(slim_v_token_end_index, INT64_C(1));
(void)slim_v_content_end;
{
SlimUnit slim_v_path = {0};
slim_v_path = slim_fn_text_append_span(slim_v_manifest_source, slim_v_content_start, slim_v_content_end, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_path;
slim_result = slim_bytes_freeze(slim_v_output);
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_project_declaration_has_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_name;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
bool slim_v_function_item = {0};
slim_v_function_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_item;
{
bool slim_v_record_item = {0};
slim_v_record_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"record", (int64_t)(sizeof("record") - 1)), slim_allocation_region);
(void)slim_v_record_item;
{
bool slim_v_variant_item = {0};
slim_v_variant_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"variant", (int64_t)(sizeof("variant") - 1)), slim_allocation_region);
(void)slim_v_variant_item;
{
bool slim_v_first = {0};
slim_v_first = slim_v_function_item || slim_v_record_item;
(void)slim_v_first;
{
bool slim_v_declaration = {0};
slim_v_declaration = slim_v_first || slim_v_variant_item;
(void)slim_v_declaration;
{
int64_t slim_v_candidate = {0};
slim_v_candidate = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_candidate;
{
int64_t slim_v_candidate_start = {0};
slim_v_candidate_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_candidate, slim_allocation_region);
(void)slim_v_candidate_start;
{
int64_t slim_v_candidate_end = {0};
slim_v_candidate_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_candidate, slim_allocation_region);
(void)slim_v_candidate_end;
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_start;
{
int64_t slim_v_name_end = {0};
slim_v_name_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_end;
{
bool slim_v_same = {0};
if (slim_v_declaration) {
slim_v_same = slim_fn_syntax_spans_equal(slim_v_source, slim_v_candidate_start, slim_v_candidate_end, slim_v_name_start, slim_v_name_end, slim_allocation_region);
}
else {
slim_v_same = false;
}
(void)slim_v_same;
if (slim_v_same) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static SlimUnit slim_fn_project_append_slash_replaced(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_index;
(void)slim_v_end;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_source, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
bool slim_v_slash = {0};
slim_v_slash = slim_v_code == INT64_C(47);
(void)slim_v_slash;
{
SlimUnit slim_v_emitted = {0};
if (slim_v_slash) {
slim_v_emitted = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"_", (int64_t)(sizeof("_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
if (!slim_vec_push(slim_v_output, &(slim_v_byte))) goto slim_allocation_failed; slim_v_emitted = (SlimUnit){0};
}
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_project_append_project_atom(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_module_items, bool slim_v_entry, int64_t slim_v_atom, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_module_name;
(void)slim_v_module_source;
(void)slim_v_module_tokens;
(void)slim_v_module_items;
(void)slim_v_entry;
(void)slim_v_atom;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_module_tokens, slim_v_atom, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_module_tokens, slim_v_atom, slim_allocation_region);
(void)slim_v_end;
{
int64_t slim_v_slashes = {0};
slim_v_slashes = slim_fn_syntax_span_count_byte(slim_v_module_source, slim_v_start, slim_v_end, INT64_C(47), INT64_C(0), slim_allocation_region);
(void)slim_v_slashes;
{
bool slim_v_qualified = {0};
slim_v_qualified = slim_v_slashes > INT64_C(0);
(void)slim_v_qualified;
if (slim_v_qualified) {
slim_result = slim_fn_project_append_slash_replaced(slim_v_module_source, slim_v_start, slim_v_end, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_declaration = {0};
slim_v_declaration = slim_fn_project_declaration_has_name(slim_v_module_source, slim_v_module_tokens, slim_v_module_items, slim_v_atom, slim_allocation_region);
(void)slim_v_declaration;
{
bool slim_v_main_name = {0};
slim_v_main_name = slim_fn_syntax_token_equal(slim_v_module_source, slim_v_module_tokens, slim_v_atom, slim_bytes_static((const uint8_t *)"main", (int64_t)(sizeof("main") - 1)), slim_allocation_region);
(void)slim_v_main_name;
{
bool slim_v_entry_main = {0};
slim_v_entry_main = slim_v_entry && slim_v_main_name;
(void)slim_v_entry_main;
{
bool slim_v_prefix = {0};
if (slim_v_declaration) {
slim_v_prefix = !slim_v_entry_main;
}
else {
slim_v_prefix = false;
}
(void)slim_v_prefix;
if (slim_v_prefix) {
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_text_append_token(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_separator = {0};
slim_v_separator = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"_", (int64_t)(sizeof("_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_separator;
slim_result = slim_fn_text_append_token(slim_v_module_source, slim_v_module_tokens, slim_v_atom, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
slim_result = slim_fn_text_append_token(slim_v_module_source, slim_v_module_tokens, slim_v_atom, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_project_append_project_tokens(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_module_items, bool slim_v_entry, int64_t slim_v_index, int64_t slim_v_end, int64_t slim_v_previous_kind, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_module_name;
(void)slim_v_module_source;
(void)slim_v_module_tokens;
(void)slim_v_module_items;
(void)slim_v_entry;
(void)slim_v_index;
(void)slim_v_end;
(void)slim_v_previous_kind;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_module_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_right = {0};
slim_v_right = slim_v_kind == INT64_C(1);
(void)slim_v_right;
{
bool slim_v_previous_left = {0};
slim_v_previous_left = slim_v_previous_kind == INT64_C(0);
(void)slim_v_previous_left;
{
bool slim_v_not_right = {0};
slim_v_not_right = !slim_v_right;
(void)slim_v_not_right;
{
bool slim_v_not_previous_left = {0};
slim_v_not_previous_left = !slim_v_previous_left;
(void)slim_v_not_previous_left;
{
bool slim_v_separated = {0};
slim_v_separated = slim_v_not_right && slim_v_not_previous_left;
(void)slim_v_separated;
{
SlimUnit slim_v_separator = {0};
if (slim_v_separated) {
slim_v_separator = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_separator = (SlimUnit){0};
}
(void)slim_v_separator;
{
bool slim_v_atom = {0};
slim_v_atom = slim_v_kind == INT64_C(2);
(void)slim_v_atom;
{
SlimUnit slim_v_emitted = {0};
if (slim_v_atom) {
slim_v_emitted = slim_fn_project_append_project_atom(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_name, slim_v_module_source, slim_v_module_tokens, slim_v_module_items, slim_v_entry, slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_emitted = slim_fn_text_append_token(slim_v_module_source, slim_v_module_tokens, slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
slim_v_previous_kind = slim_v_kind;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_project_append_project_modules(SlimBytes slim_v_manifest_path, SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_entry_name, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_path;
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_entry_name;
(void)slim_v_cursor;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_manifest_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_module_name = {0};
slim_v_module_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_module_name;
{
int64_t slim_v_path_token = {0};
slim_v_path_token = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_path_token;
{
SlimBytes slim_v_module_path = {0};
slim_v_module_path = slim_fn_project_project_module_path(slim_v_manifest_path, slim_v_manifest_source, slim_v_manifest_tokens, slim_v_path_token, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_path;
{
SlimVec slim_v_module_source_bytes = {0};
slim_v_module_source_bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_module_source_bytes;
{
bool slim_v_module_source_read = {0};
slim_v_module_source_read = slim_read_file(slim_v_module_path, &slim_v_module_source_bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_source_read;
{
SlimBytes slim_v_module_source = {0};
slim_v_module_source = slim_bytes_freeze(slim_v_module_source_bytes);
(void)slim_v_module_source;
{
SlimVec slim_v_module_tokens = {0};
slim_v_module_tokens = slim_vec_new(sizeof(Slim_type_syntax_Token), slim_allocation_region);
(void)slim_v_module_tokens;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_lex(slim_v_module_source, INT64_C(0), &slim_v_module_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
{
int64_t slim_v_module_name_start = {0};
slim_v_module_name_start = slim_fn_syntax_token_start(slim_v_manifest_tokens, slim_v_module_name, slim_allocation_region);
(void)slim_v_module_name_start;
{
int64_t slim_v_module_name_end = {0};
slim_v_module_name_end = slim_fn_syntax_token_end(slim_v_manifest_tokens, slim_v_module_name, slim_allocation_region);
(void)slim_v_module_name_end;
{
int64_t slim_v_source_name = {0};
slim_v_source_name = INT64_C(2);
(void)slim_v_source_name;
{
int64_t slim_v_source_name_start = {0};
slim_v_source_name_start = slim_fn_syntax_token_start(&slim_v_module_tokens, slim_v_source_name, slim_allocation_region);
(void)slim_v_source_name_start;
{
int64_t slim_v_source_name_end = {0};
slim_v_source_name_end = slim_fn_syntax_token_end(&slim_v_module_tokens, slim_v_source_name, slim_allocation_region);
(void)slim_v_source_name_end;
{
bool slim_v_module_identity = {0};
slim_v_module_identity = slim_fn_project_cross_spans_equal(slim_v_manifest_source, slim_v_module_name_start, slim_v_module_name_end, slim_v_module_source, slim_v_source_name_start, slim_v_source_name_end, slim_allocation_region);
(void)slim_v_module_identity;
{
int64_t slim_v_entry_name_start = {0};
slim_v_entry_name_start = slim_fn_syntax_token_start(slim_v_manifest_tokens, slim_v_entry_name, slim_allocation_region);
(void)slim_v_entry_name_start;
{
int64_t slim_v_entry_name_end = {0};
slim_v_entry_name_end = slim_fn_syntax_token_end(slim_v_manifest_tokens, slim_v_entry_name, slim_allocation_region);
(void)slim_v_entry_name_end;
{
bool slim_v_entry = {0};
slim_v_entry = slim_fn_syntax_spans_equal(slim_v_manifest_source, slim_v_module_name_start, slim_v_module_name_end, slim_v_entry_name_start, slim_v_entry_name_end, slim_allocation_region);
(void)slim_v_entry;
{
int64_t slim_v_module_items = {0};
slim_v_module_items = INT64_C(3);
(void)slim_v_module_items;
{
int64_t slim_v_length = {0};
slim_v_length = (slim_v_module_tokens).len;
(void)slim_v_length;
{
int64_t slim_v_end = {0};
slim_v_end = slim_i64_sub(slim_v_length, INT64_C(1));
(void)slim_v_end;
{
SlimUnit slim_v_appended = {0};
slim_v_appended = slim_fn_project_append_project_tokens(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_name, slim_v_module_source, &slim_v_module_tokens, slim_v_module_items, slim_v_entry, slim_v_module_items, slim_v_end, INT64_C(2), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_appended;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_manifest_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimBytes slim_fn_project_flatten_project(SlimBytes slim_v_path, SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimBytes slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_entry_form = {0};
slim_v_entry_form = INT64_C(3);
(void)slim_v_entry_form;
{
int64_t slim_v_entry_name = {0};
slim_v_entry_name = slim_i64_add(slim_v_entry_form, INT64_C(2));
(void)slim_v_entry_name;
{
int64_t slim_v_first_module = {0};
slim_v_first_module = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_entry_form, slim_allocation_region);
(void)slim_v_first_module;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_opened = {0};
slim_v_opened = slim_fn_text_append_text(&slim_v_output, slim_bytes_static((const uint8_t *)"(module project", (int64_t)(sizeof("(module project") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_opened;
{
SlimUnit slim_v_modules = {0};
slim_v_modules = slim_fn_project_append_project_modules(slim_v_path, slim_v_source, slim_v_tokens, slim_v_entry_name, slim_v_first_module, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_modules;
{
SlimUnit slim_v_closed = {0};
slim_v_closed = slim_fn_text_append_text(&slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_closed;
slim_result = slim_bytes_freeze(slim_v_output);
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_project_path_is_project(SlimBytes slim_v_path, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_source_bytes = {0};
slim_v_source_bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_source_bytes;
{
bool slim_v_source_read = {0};
slim_v_source_read = slim_read_file(slim_v_path, &slim_v_source_bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_source_read;
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_bytes_freeze(slim_v_source_bytes);
(void)slim_v_source;
{
SlimVec slim_v_tokens = {0};
slim_v_tokens = slim_vec_new(sizeof(Slim_type_syntax_Token), slim_allocation_region);
(void)slim_v_tokens;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_lex(slim_v_source, INT64_C(0), &slim_v_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
slim_result = slim_fn_syntax_token_equal(slim_v_source, &slim_v_tokens, INT64_C(1), slim_bytes_static((const uint8_t *)"project", (int64_t)(sizeof("project") - 1)), slim_allocation_region);
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static SlimBytes slim_fn_project_generate_project_path(SlimBytes slim_v_path, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimBytes slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_source_bytes = {0};
slim_v_source_bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_source_bytes;
{
bool slim_v_source_read = {0};
slim_v_source_read = slim_read_file(slim_v_path, &slim_v_source_bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_source_read;
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_bytes_freeze(slim_v_source_bytes);
(void)slim_v_source;
{
SlimVec slim_v_tokens = {0};
slim_v_tokens = slim_vec_new(sizeof(Slim_type_syntax_Token), slim_allocation_region);
(void)slim_v_tokens;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_lex(slim_v_source, INT64_C(0), &slim_v_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
{
SlimBytes slim_v_flattened = {0};
slim_v_flattened = slim_fn_project_flatten_project(slim_v_path, slim_v_source, &slim_v_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_flattened;
{
SlimVec slim_v_flat_tokens = {0};
slim_v_flat_tokens = slim_vec_new(sizeof(Slim_type_syntax_Token), slim_allocation_region);
(void)slim_v_flat_tokens;
{
SlimUnit slim_v_flat_tokenized = {0};
slim_v_flat_tokenized = slim_fn_syntax_lex(slim_v_flattened, INT64_C(0), &slim_v_flat_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_flat_tokenized;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_emit_program(slim_v_flattened, &slim_v_flat_tokens, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = slim_bytes_freeze(slim_v_output);
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_compile_project_path_jobs(SlimBytes slim_v_path, int64_t slim_v_jobs, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_v_jobs;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_checked = {0};
slim_v_checked = slim_fn_project_check_project_path(slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_checked;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_checked > INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
slim_result = slim_v_checked;
}
else {
{
SlimVec slim_v_manifest_bytes = {0};
slim_v_manifest_bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_manifest_bytes;
{
bool slim_v_manifest_read = {0};
slim_v_manifest_read = slim_read_file(slim_v_path, &slim_v_manifest_bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_manifest_read;
{
SlimBytes slim_v_manifest_source = {0};
slim_v_manifest_source = slim_bytes_freeze(slim_v_manifest_bytes);
(void)slim_v_manifest_source;
{
SlimVec slim_v_manifest_tokens = {0};
slim_v_manifest_tokens = slim_vec_new(sizeof(Slim_type_syntax_Token), slim_allocation_region);
(void)slim_v_manifest_tokens;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_lex(slim_v_manifest_source, INT64_C(0), &slim_v_manifest_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
{
Slim_type_scheduler_Schedule slim_v_schedule = {0};
slim_v_schedule = slim_fn_scheduler_plan(slim_v_manifest_source, &slim_v_manifest_tokens, slim_v_jobs, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_schedule;
{
bool slim_v_schedule_valid = {0};
slim_v_schedule_valid = slim_v_schedule.slim_field_valid;
(void)slim_v_schedule_valid;
if (!slim_v_schedule_valid) {
slim_result = INT64_C(70);
}
else {
{
SlimBytes slim_v_generated = {0};
slim_v_generated = slim_fn_project_generate_project_path(slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_generated;
{
SlimUnit slim_v_shown = {0};
slim_v_shown = slim_print_bytes(slim_v_generated);
(void)slim_v_shown;
slim_result = INT64_C(0);
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_project_compile_project_path(SlimBytes slim_v_path, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_result = slim_fn_project_compile_project_path_jobs(slim_v_path, INT64_C(1), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static SlimBytes slim_fn_project_token_bytes(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimBytes slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_end;
{
SlimUnit slim_v_appended = {0};
slim_v_appended = slim_fn_text_append_span(slim_v_source, slim_v_start, slim_v_end, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_appended;
slim_result = slim_bytes_freeze(slim_v_output);
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_report_project_diagnostic(SlimBytes slim_v_code, SlimBytes slim_v_module_name, int64_t slim_v_start, int64_t slim_v_end, SlimRegion *slim_region) {
(void)slim_v_code;
(void)slim_v_module_name;
(void)slim_v_start;
(void)slim_v_end;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_code_text = {0};
slim_v_code_text = slim_fn_text_append_text(&slim_v_output, slim_v_code, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_code_text;
{
SlimUnit slim_v_first_at = {0};
slim_v_first_at = slim_fn_text_append_text(&slim_v_output, slim_bytes_static((const uint8_t *)"@", (int64_t)(sizeof("@") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_first_at;
{
SlimUnit slim_v_module_text = {0};
slim_v_module_text = slim_fn_text_append_text(&slim_v_output, slim_v_module_name, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_text;
{
SlimUnit slim_v_second_at = {0};
slim_v_second_at = slim_fn_text_append_text(&slim_v_output, slim_bytes_static((const uint8_t *)"@", (int64_t)(sizeof("@") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_second_at;
{
SlimUnit slim_v_start_text = {0};
slim_v_start_text = slim_fn_text_append_nonnegative_i64(slim_v_start, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_start_text;
{
SlimUnit slim_v_colon = {0};
slim_v_colon = slim_fn_text_append_text(&slim_v_output, slim_bytes_static((const uint8_t *)":", (int64_t)(sizeof(":") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_colon;
{
SlimUnit slim_v_end_text = {0};
slim_v_end_text = slim_fn_text_append_nonnegative_i64(slim_v_end, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_end_text;
{
SlimUnit slim_v_newline = {0};
slim_v_newline = slim_fn_text_append_text(&slim_v_output, slim_bytes_static((const uint8_t *)"\n", (int64_t)(sizeof("\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_newline;
{
SlimBytes slim_v_bytes = {0};
slim_v_bytes = slim_bytes_freeze(slim_v_output);
(void)slim_v_bytes;
{
SlimUnit slim_v_shown = {0};
slim_v_shown = slim_print_bytes(slim_v_bytes);
(void)slim_v_shown;
slim_result = INT64_C(1);
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_project_imports_has_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_name_start;
(void)slim_v_name_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
int64_t slim_v_candidate_start = {0};
slim_v_candidate_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_candidate_start;
{
int64_t slim_v_candidate_end = {0};
slim_v_candidate_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_candidate_end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_spans_equal(slim_v_source, slim_v_candidate_start, slim_v_candidate_end, slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_find_manifest_module(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_name_start;
(void)slim_v_name_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_module_name = {0};
slim_v_module_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_module_name;
{
int64_t slim_v_candidate_start = {0};
slim_v_candidate_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_module_name, slim_allocation_region);
(void)slim_v_candidate_start;
{
int64_t slim_v_candidate_end = {0};
slim_v_candidate_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_module_name, slim_allocation_region);
(void)slim_v_candidate_end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_spans_equal(slim_v_source, slim_v_candidate_start, slim_v_candidate_end, slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_project_module_imports_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_module_name, int64_t slim_v_import_start, int64_t slim_v_import_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_modules;
(void)slim_v_module_name;
(void)slim_v_import_start;
(void)slim_v_import_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_module_name, slim_allocation_region);
(void)slim_v_name_start;
{
int64_t slim_v_name_end = {0};
slim_v_name_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_module_name, slim_allocation_region);
(void)slim_v_name_end;
{
int64_t slim_v_module_form = {0};
slim_v_module_form = slim_fn_project_find_manifest_module(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_module_form;
{
bool slim_v_found = {0};
slim_v_found = slim_v_module_form >= INT64_C(0);
(void)slim_v_found;
if (!slim_v_found) {
slim_result = false;
}
else {
{
int64_t slim_v_imports_form = {0};
slim_v_imports_form = slim_i64_add(slim_v_module_form, INT64_C(4));
(void)slim_v_imports_form;
{
int64_t slim_v_first_import = {0};
slim_v_first_import = slim_i64_add(slim_v_imports_form, INT64_C(2));
(void)slim_v_first_import;
slim_result = slim_fn_project_imports_has_name(slim_v_source, slim_v_tokens, slim_v_first_import, slim_v_import_start, slim_v_import_end, slim_allocation_region);
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_project_module_cycle_imports(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_module_name, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_modules;
(void)slim_v_module_name;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
int64_t slim_v_import_start = {0};
slim_v_import_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_import_start;
{
int64_t slim_v_import_end = {0};
slim_v_import_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_import_end;
{
bool slim_v_reciprocal = {0};
slim_v_reciprocal = slim_fn_project_module_imports_name(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_cursor, slim_v_import_start, slim_v_import_end, slim_allocation_region);
(void)slim_v_reciprocal;
{
int64_t slim_v_module_name_start = {0};
slim_v_module_name_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_module_name, slim_allocation_region);
(void)slim_v_module_name_start;
{
int64_t slim_v_module_name_end = {0};
slim_v_module_name_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_module_name, slim_allocation_region);
(void)slim_v_module_name_end;
{
int64_t slim_v_target_form = {0};
slim_v_target_form = slim_fn_project_find_manifest_module(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_import_start, slim_v_import_end, slim_allocation_region);
(void)slim_v_target_form;
{
bool slim_v_target_found = {0};
slim_v_target_found = slim_v_target_form >= INT64_C(0);
(void)slim_v_target_found;
{
bool slim_v_imports_source = {0};
if (slim_v_target_found) {
{
int64_t slim_v_target_imports = {0};
slim_v_target_imports = slim_i64_add(slim_v_target_form, INT64_C(4));
(void)slim_v_target_imports;
{
int64_t slim_v_first_target_import = {0};
slim_v_first_target_import = slim_i64_add(slim_v_target_imports, INT64_C(2));
(void)slim_v_first_target_import;
slim_v_imports_source = slim_fn_project_imports_has_name(slim_v_source, slim_v_tokens, slim_v_first_target_import, slim_v_module_name_start, slim_v_module_name_end, slim_allocation_region);
}
}
}
else {
slim_v_imports_source = false;
}
(void)slim_v_imports_source;
if (slim_v_imports_source) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_find_reciprocal_cycle(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_modules;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_module_name = {0};
slim_v_module_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_module_name;
{
int64_t slim_v_imports_form = {0};
slim_v_imports_form = slim_i64_add(slim_v_cursor, INT64_C(4));
(void)slim_v_imports_form;
{
int64_t slim_v_first_import = {0};
slim_v_first_import = slim_i64_add(slim_v_imports_form, INT64_C(2));
(void)slim_v_first_import;
{
bool slim_v_cycle = {0};
slim_v_cycle = slim_fn_project_module_cycle_imports(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_module_name, slim_v_first_import, slim_allocation_region);
(void)slim_v_cycle;
if (slim_v_cycle) {
slim_result = slim_v_module_name;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_find_span_slash(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_index;
(void)slim_v_end;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_source, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
bool slim_v_slash = {0};
slim_v_slash = slim_v_code == INT64_C(47);
(void)slim_v_slash;
if (slim_v_slash) {
slim_result = slim_v_index;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_project_exports_has_segment(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_cursor, SlimBytes slim_v_module_source, int64_t slim_v_segment_start, int64_t slim_v_segment_end, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_cursor;
(void)slim_v_module_source;
(void)slim_v_segment_start;
(void)slim_v_segment_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_manifest_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
int64_t slim_v_export_start = {0};
slim_v_export_start = slim_fn_syntax_token_start(slim_v_manifest_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_export_start;
{
int64_t slim_v_export_end = {0};
slim_v_export_end = slim_fn_syntax_token_end(slim_v_manifest_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_export_end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_project_cross_spans_equal(slim_v_manifest_source, slim_v_export_start, slim_v_export_end, slim_v_module_source, slim_v_segment_start, slim_v_segment_end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_check_project_path(SlimBytes slim_v_path, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_source_bytes = {0};
slim_v_source_bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_source_bytes;
{
bool slim_v_source_read = {0};
slim_v_source_read = slim_read_file(slim_v_path, &slim_v_source_bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_source_read;
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_bytes_freeze(slim_v_source_bytes);
(void)slim_v_source;
{
SlimVec slim_v_tokens = {0};
slim_v_tokens = slim_vec_new(sizeof(Slim_type_syntax_Token), slim_allocation_region);
(void)slim_v_tokens;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_lex(slim_v_source, INT64_C(0), &slim_v_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
{
int64_t slim_v_version = {0};
slim_v_version = INT64_C(2);
(void)slim_v_version;
{
bool slim_v_valid_version = {0};
slim_v_valid_version = slim_fn_syntax_token_equal(slim_v_source, &slim_v_tokens, slim_v_version, slim_bytes_static((const uint8_t *)"1", (int64_t)(sizeof("1") - 1)), slim_allocation_region);
(void)slim_v_valid_version;
if (!slim_v_valid_version) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(&slim_v_tokens, slim_v_version, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(&slim_v_tokens, slim_v_version, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_project_report_project_diagnostic(slim_bytes_static((const uint8_t *)"E0402", (int64_t)(sizeof("E0402") - 1)), slim_bytes_static((const uint8_t *)"-", (int64_t)(sizeof("-") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
{
int64_t slim_v_entry_form = {0};
slim_v_entry_form = INT64_C(3);
(void)slim_v_entry_form;
{
int64_t slim_v_entry = {0};
slim_v_entry = slim_i64_add(slim_v_entry_form, INT64_C(2));
(void)slim_v_entry;
{
int64_t slim_v_modules = {0};
slim_v_modules = slim_fn_syntax_skip_form(&slim_v_tokens, slim_v_entry_form, slim_allocation_region);
(void)slim_v_modules;
{
int64_t slim_v_manifest_errors = {0};
slim_v_manifest_errors = slim_fn_project_report_manifest_rules(slim_v_source, &slim_v_tokens, slim_v_entry_form, slim_v_modules, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_manifest_errors;
{
bool slim_v_invalid_manifest = {0};
slim_v_invalid_manifest = slim_v_manifest_errors > INT64_C(0);
(void)slim_v_invalid_manifest;
if (slim_v_invalid_manifest) {
slim_result = slim_v_manifest_errors;
}
else {
{
int64_t slim_v_cycle = {0};
slim_v_cycle = slim_fn_project_find_reciprocal_cycle(slim_v_source, &slim_v_tokens, slim_v_modules, slim_v_modules, slim_allocation_region);
(void)slim_v_cycle;
{
bool slim_v_has_cycle = {0};
slim_v_has_cycle = slim_v_cycle >= INT64_C(0);
(void)slim_v_has_cycle;
if (slim_v_has_cycle) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(&slim_v_tokens, slim_v_cycle, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(&slim_v_tokens, slim_v_cycle, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_project_report_project_diagnostic(slim_bytes_static((const uint8_t *)"E0413", (int64_t)(sizeof("E0413") - 1)), slim_bytes_static((const uint8_t *)"-", (int64_t)(sizeof("-") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
{
SlimVec slim_v_module_tokens = {0};
slim_v_module_tokens = slim_vec_new(sizeof(Slim_type_syntax_Token), slim_allocation_region);
(void)slim_v_module_tokens;
{
SlimVec slim_v_loaded = {0};
slim_v_loaded = slim_vec_new(sizeof(Slim_type_project_LoadedModule), slim_allocation_region);
(void)slim_v_loaded;
{
int64_t slim_v_loaded_modules = {0};
slim_v_loaded_modules = slim_fn_project_load_project_modules(slim_v_path, slim_v_source, &slim_v_tokens, slim_v_modules, &slim_v_module_tokens, &slim_v_loaded, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_loaded_modules;
{
bool slim_v_invalid_loading = {0};
slim_v_invalid_loading = slim_v_loaded_modules > INT64_C(0);
(void)slim_v_invalid_loading;
if (slim_v_invalid_loading) {
slim_result = slim_v_loaded_modules;
}
else {
{
int64_t slim_v_project_errors = {0};
slim_v_project_errors = slim_fn_project_report_loaded_project(slim_v_source, &slim_v_tokens, slim_v_entry, &slim_v_module_tokens, &slim_v_loaded, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_project_errors;
{
bool slim_v_invalid_project = {0};
slim_v_invalid_project = slim_v_project_errors > INT64_C(0);
(void)slim_v_invalid_project;
if (slim_v_invalid_project) {
slim_result = slim_v_project_errors;
}
else {
slim_result = INT64_C(0);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static SlimUnit slim_fn_project_append_interface_type(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_type_index, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_module_name;
(void)slim_v_module_source;
(void)slim_v_module_tokens;
(void)slim_v_type_index;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_module_tokens, slim_v_type_index, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_composite = {0};
slim_v_composite = slim_v_kind == INT64_C(0);
(void)slim_v_composite;
if (slim_v_composite) {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_type_index, INT64_C(1));
(void)slim_v_head;
{
int64_t slim_v_inner = {0};
slim_v_inner = slim_i64_add(slim_v_type_index, INT64_C(2));
(void)slim_v_inner;
{
SlimUnit slim_v_opened = {0};
slim_v_opened = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"(", (int64_t)(sizeof("(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_opened;
{
SlimUnit slim_v_constructor = {0};
slim_v_constructor = slim_fn_text_append_token(slim_v_module_source, slim_v_module_tokens, slim_v_head, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_constructor;
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_nested = {0};
slim_v_nested = slim_fn_project_append_interface_type(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_name, slim_v_module_source, slim_v_module_tokens, slim_v_inner, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_nested;
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
else {
{
int64_t slim_v_scalar_kind = {0};
slim_v_scalar_kind = slim_fn_syntax_scalar_type_kind(slim_v_module_source, slim_v_module_tokens, slim_v_type_index, slim_allocation_region);
(void)slim_v_scalar_kind;
{
bool slim_v_scalar = {0};
slim_v_scalar = slim_v_scalar_kind >= INT64_C(0);
(void)slim_v_scalar;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_module_tokens, slim_v_type_index, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_module_tokens, slim_v_type_index, slim_allocation_region);
(void)slim_v_end;
{
int64_t slim_v_slashes = {0};
slim_v_slashes = slim_fn_syntax_span_count_byte(slim_v_module_source, slim_v_start, slim_v_end, INT64_C(47), INT64_C(0), slim_allocation_region);
(void)slim_v_slashes;
{
bool slim_v_qualified = {0};
slim_v_qualified = slim_v_slashes > INT64_C(0);
(void)slim_v_qualified;
{
bool slim_v_raw = {0};
slim_v_raw = slim_v_scalar || slim_v_qualified;
(void)slim_v_raw;
if (slim_v_raw) {
slim_result = slim_fn_text_append_token(slim_v_module_source, slim_v_module_tokens, slim_v_type_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_append_token(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_slash = {0};
slim_v_slash = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"/", (int64_t)(sizeof("/") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_slash;
slim_result = slim_fn_text_append_token(slim_v_module_source, slim_v_module_tokens, slim_v_type_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_project_append_interface_parameters(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_module_name;
(void)slim_v_module_source;
(void)slim_v_module_tokens;
(void)slim_v_cursor;
(void)slim_v_first;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
SlimUnit slim_v_separator = {0};
if (slim_v_first) {
slim_v_separator = (SlimUnit){0};
}
else {
slim_v_separator = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_separator;
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_token_equal(slim_v_module_source, slim_v_module_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"inout", (int64_t)(sizeof("inout") - 1)), slim_allocation_region);
(void)slim_v_borrowed;
{
int64_t slim_v_type_index = {0};
if (slim_v_borrowed) {
slim_v_type_index = slim_i64_add(slim_v_cursor, INT64_C(3));
}
else {
slim_v_type_index = slim_i64_add(slim_v_cursor, INT64_C(2));
}
(void)slim_v_type_index;
{
SlimUnit slim_v_opened = {0};
slim_v_opened = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"(", (int64_t)(sizeof("(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_opened;
{
SlimUnit slim_v_mode = {0};
if (slim_v_borrowed) {
slim_v_mode = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"inout ", (int64_t)(sizeof("inout ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_mode = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"owned ", (int64_t)(sizeof("owned ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_mode;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_project_append_interface_type(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_name, slim_v_module_source, slim_v_module_tokens, slim_v_type_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_closed = {0};
slim_v_closed = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_closed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
slim_v_first = false;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_project_append_interface_effects(SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_module_source;
(void)slim_v_module_tokens;
(void)slim_v_cursor;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_effect = {0};
slim_v_effect = slim_fn_text_append_token(slim_v_module_source, slim_v_module_tokens, slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_effect;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_find_export_declaration(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_export_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_export_name;
(void)slim_v_module_source;
(void)slim_v_module_tokens;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_export_start = {0};
slim_v_export_start = slim_fn_syntax_token_start(slim_v_manifest_tokens, slim_v_export_name, slim_allocation_region);
(void)slim_v_export_start;
{
int64_t slim_v_export_end = {0};
slim_v_export_end = slim_fn_syntax_token_end(slim_v_manifest_tokens, slim_v_export_name, slim_allocation_region);
(void)slim_v_export_end;
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_fn_syntax_token_start(slim_v_module_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_start;
{
int64_t slim_v_name_end = {0};
slim_v_name_end = slim_fn_syntax_token_end(slim_v_module_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_project_cross_spans_equal(slim_v_manifest_source, slim_v_export_start, slim_v_export_end, slim_v_module_source, slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static SlimUnit slim_fn_project_append_interface_fields(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_module_name;
(void)slim_v_module_source;
(void)slim_v_module_tokens;
(void)slim_v_cursor;
(void)slim_v_first;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
SlimUnit slim_v_separator = {0};
if (slim_v_first) {
slim_v_separator = (SlimUnit){0};
}
else {
slim_v_separator = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_separator;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_name;
{
int64_t slim_v_type_index = {0};
slim_v_type_index = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_type_index;
{
SlimUnit slim_v_opened = {0};
slim_v_opened = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"(", (int64_t)(sizeof("(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_opened;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_text_append_token(slim_v_module_source, slim_v_module_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_project_append_interface_type(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_name, slim_v_module_source, slim_v_module_tokens, slim_v_type_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_closed = {0};
slim_v_closed = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_closed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
slim_v_first = false;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_project_append_interface_case_types(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_module_name;
(void)slim_v_module_source;
(void)slim_v_module_tokens;
(void)slim_v_cursor;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_project_append_interface_type(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_name, slim_v_module_source, slim_v_module_tokens, slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_project_append_interface_cases(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_module_name;
(void)slim_v_module_source;
(void)slim_v_module_tokens;
(void)slim_v_cursor;
(void)slim_v_first;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
SlimUnit slim_v_separator = {0};
if (slim_v_first) {
slim_v_separator = (SlimUnit){0};
}
else {
slim_v_separator = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_separator;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_name;
{
int64_t slim_v_first_type = {0};
slim_v_first_type = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_first_type;
{
SlimUnit slim_v_opened = {0};
slim_v_opened = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"(", (int64_t)(sizeof("(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_opened;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_text_append_token(slim_v_module_source, slim_v_module_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_types = {0};
slim_v_types = slim_fn_project_append_interface_case_types(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_name, slim_v_module_source, slim_v_module_tokens, slim_v_first_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_types;
{
SlimUnit slim_v_closed = {0};
slim_v_closed = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_closed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
slim_v_first = false;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_project_append_interface_declaration(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_item, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_module_name;
(void)slim_v_module_source;
(void)slim_v_module_tokens;
(void)slim_v_item;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_function_item = {0};
slim_v_function_item = slim_fn_syntax_item_is(slim_v_module_source, slim_v_module_tokens, slim_v_item, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_item;
if (slim_v_function_item) {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_item, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_item, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_type = {0};
slim_v_return_type = slim_fn_syntax_skip_form(slim_v_module_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_type;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_skip_form(slim_v_module_tokens, slim_v_return_type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_first_effect = {0};
slim_v_first_effect = slim_i64_add(slim_v_effects, INT64_C(2));
(void)slim_v_first_effect;
{
SlimUnit slim_v_opened = {0};
slim_v_opened = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"(fn ", (int64_t)(sizeof("(fn ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_opened;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_text_append_token(slim_v_module_source, slim_v_module_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_params_open = {0};
slim_v_params_open = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" (", (int64_t)(sizeof(" (") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_params_open;
{
int64_t slim_v_first_parameter = {0};
slim_v_first_parameter = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_first_parameter;
{
SlimUnit slim_v_parameters = {0};
slim_v_parameters = slim_fn_project_append_interface_parameters(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_name, slim_v_module_source, slim_v_module_tokens, slim_v_first_parameter, true, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_parameters;
{
SlimUnit slim_v_return_space = {0};
slim_v_return_space = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_return_space;
{
SlimUnit slim_v_returned = {0};
slim_v_returned = slim_fn_project_append_interface_type(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_name, slim_v_module_source, slim_v_module_tokens, slim_v_return_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_returned;
{
SlimUnit slim_v_effect_space = {0};
slim_v_effect_space = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" (effects", (int64_t)(sizeof(" (effects") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_effect_space;
{
SlimUnit slim_v_emitted_effects = {0};
slim_v_emitted_effects = slim_fn_project_append_interface_effects(slim_v_module_source, slim_v_module_tokens, slim_v_first_effect, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted_effects;
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
else {
{
bool slim_v_record_item = {0};
slim_v_record_item = slim_fn_syntax_item_is(slim_v_module_source, slim_v_module_tokens, slim_v_item, slim_bytes_static((const uint8_t *)"record", (int64_t)(sizeof("record") - 1)), slim_allocation_region);
(void)slim_v_record_item;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_item, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_body = {0};
slim_v_body = slim_i64_add(slim_v_item, INT64_C(3));
(void)slim_v_body;
{
int64_t slim_v_first_member = {0};
slim_v_first_member = slim_i64_add(slim_v_body, INT64_C(1));
(void)slim_v_first_member;
{
SlimUnit slim_v_opened = {0};
if (slim_v_record_item) {
slim_v_opened = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"(record ", (int64_t)(sizeof("(record ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_opened = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"(variant ", (int64_t)(sizeof("(variant ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_opened;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_text_append_token(slim_v_module_source, slim_v_module_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_members_open = {0};
slim_v_members_open = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" (", (int64_t)(sizeof(" (") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_members_open;
{
SlimUnit slim_v_members = {0};
if (slim_v_record_item) {
slim_v_members = slim_fn_project_append_interface_fields(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_name, slim_v_module_source, slim_v_module_tokens, slim_v_first_member, true, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_members = slim_fn_project_append_interface_cases(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_name, slim_v_module_source, slim_v_module_tokens, slim_v_first_member, true, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_members;
slim_result = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_project_append_interface_exports(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_module_name, int64_t slim_v_cursor, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_module_items, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_module_name;
(void)slim_v_cursor;
(void)slim_v_module_source;
(void)slim_v_module_tokens;
(void)slim_v_module_items;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_manifest_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_item = {0};
slim_v_item = slim_fn_project_find_export_declaration(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_cursor, slim_v_module_source, slim_v_module_tokens, slim_v_module_items, slim_allocation_region);
(void)slim_v_item;
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_declaration = {0};
slim_v_declaration = slim_fn_project_append_interface_declaration(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_name, slim_v_module_source, slim_v_module_tokens, slim_v_item, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_declaration;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_project_append_project_interfaces(SlimBytes slim_v_manifest_path, SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_path;
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_cursor;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_manifest_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_module_name = {0};
slim_v_module_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_module_name;
{
int64_t slim_v_path_token = {0};
slim_v_path_token = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_path_token;
{
int64_t slim_v_imports_form = {0};
slim_v_imports_form = slim_i64_add(slim_v_cursor, INT64_C(4));
(void)slim_v_imports_form;
{
int64_t slim_v_exports_form = {0};
slim_v_exports_form = slim_fn_syntax_skip_form(slim_v_manifest_tokens, slim_v_imports_form, slim_allocation_region);
(void)slim_v_exports_form;
{
int64_t slim_v_first_export = {0};
slim_v_first_export = slim_i64_add(slim_v_exports_form, INT64_C(2));
(void)slim_v_first_export;
{
SlimBytes slim_v_module_path = {0};
slim_v_module_path = slim_fn_project_project_module_path(slim_v_manifest_path, slim_v_manifest_source, slim_v_manifest_tokens, slim_v_path_token, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_path;
{
SlimVec slim_v_module_source_bytes = {0};
slim_v_module_source_bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_module_source_bytes;
{
bool slim_v_module_source_read = {0};
slim_v_module_source_read = slim_read_file(slim_v_module_path, &slim_v_module_source_bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_source_read;
{
SlimBytes slim_v_module_source = {0};
slim_v_module_source = slim_bytes_freeze(slim_v_module_source_bytes);
(void)slim_v_module_source;
{
SlimVec slim_v_module_tokens = {0};
slim_v_module_tokens = slim_vec_new(sizeof(Slim_type_syntax_Token), slim_allocation_region);
(void)slim_v_module_tokens;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_lex(slim_v_module_source, INT64_C(0), &slim_v_module_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
{
SlimUnit slim_v_opened = {0};
slim_v_opened = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)"(interface 1 ", (int64_t)(sizeof("(interface 1 ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_opened;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_text_append_token(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
int64_t slim_v_module_items = {0};
slim_v_module_items = INT64_C(3);
(void)slim_v_module_items;
{
SlimUnit slim_v_exports = {0};
slim_v_exports = slim_fn_project_append_interface_exports(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_name, slim_v_first_export, slim_v_module_source, &slim_v_module_tokens, slim_v_module_items, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_exports;
{
SlimUnit slim_v_closed = {0};
slim_v_closed = slim_fn_text_append_text(slim_v_output, slim_bytes_static((const uint8_t *)")\n", (int64_t)(sizeof(")\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_closed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_manifest_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_interfaces_path(SlimBytes slim_v_path, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_checked = {0};
slim_v_checked = slim_fn_project_check_project_path(slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_checked;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_checked > INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
slim_result = slim_v_checked;
}
else {
{
SlimVec slim_v_source_bytes = {0};
slim_v_source_bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_source_bytes;
{
bool slim_v_source_read = {0};
slim_v_source_read = slim_read_file(slim_v_path, &slim_v_source_bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_source_read;
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_bytes_freeze(slim_v_source_bytes);
(void)slim_v_source;
{
SlimVec slim_v_tokens = {0};
slim_v_tokens = slim_vec_new(sizeof(Slim_type_syntax_Token), slim_allocation_region);
(void)slim_v_tokens;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_lex(slim_v_source, INT64_C(0), &slim_v_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
{
int64_t slim_v_entry_form = {0};
slim_v_entry_form = INT64_C(3);
(void)slim_v_entry_form;
{
int64_t slim_v_modules = {0};
slim_v_modules = slim_fn_syntax_skip_form(&slim_v_tokens, slim_v_entry_form, slim_allocation_region);
(void)slim_v_modules;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_project_append_project_interfaces(slim_v_path, slim_v_source, &slim_v_tokens, slim_v_modules, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
{
SlimBytes slim_v_bytes = {0};
slim_v_bytes = slim_bytes_freeze(slim_v_output);
(void)slim_v_bytes;
{
SlimUnit slim_v_shown = {0};
slim_v_shown = slim_print_bytes(slim_v_bytes);
(void)slim_v_shown;
slim_result = INT64_C(0);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_project_span_less_chars(SlimBytes slim_v_source, int64_t slim_v_left, int64_t slim_v_left_end, int64_t slim_v_right, int64_t slim_v_right_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_left;
(void)slim_v_left_end;
(void)slim_v_right;
(void)slim_v_right_end;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
bool slim_v_left_done = {0};
slim_v_left_done = slim_v_left >= slim_v_left_end;
(void)slim_v_left_done;
{
bool slim_v_right_done = {0};
slim_v_right_done = slim_v_right >= slim_v_right_end;
(void)slim_v_right_done;
if (slim_v_left_done) {
slim_result = !slim_v_right_done;
}
else {
if (slim_v_right_done) {
slim_result = false;
}
else {
{
uint8_t slim_v_left_byte = {0};
slim_v_left_byte = slim_bytes_get(slim_v_source, slim_v_left);
(void)slim_v_left_byte;
{
uint8_t slim_v_right_byte = {0};
slim_v_right_byte = slim_bytes_get(slim_v_source, slim_v_right);
(void)slim_v_right_byte;
{
int64_t slim_v_left_code = {0};
slim_v_left_code = (int64_t)slim_v_left_byte;
(void)slim_v_left_code;
{
int64_t slim_v_right_code = {0};
slim_v_right_code = (int64_t)slim_v_right_byte;
(void)slim_v_right_code;
{
bool slim_v_equal = {0};
slim_v_equal = slim_v_left_code == slim_v_right_code;
(void)slim_v_equal;
if (slim_v_equal) {
{
int64_t slim_v_next_left = {0};
slim_v_next_left = slim_i64_add(slim_v_left, INT64_C(1));
(void)slim_v_next_left;
{
int64_t slim_v_next_right = {0};
slim_v_next_right = slim_i64_add(slim_v_right, INT64_C(1));
(void)slim_v_next_right;
slim_v_left = slim_v_next_left;
slim_v_right = slim_v_next_right;
goto slim_recur;
}
}
}
else {
slim_result = slim_v_left_code < slim_v_right_code;
}
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_project_span_less(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_left, int64_t slim_v_right, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_left;
(void)slim_v_right;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_left_start = {0};
slim_v_left_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_left, slim_allocation_region);
(void)slim_v_left_start;
{
int64_t slim_v_left_end = {0};
slim_v_left_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_left, slim_allocation_region);
(void)slim_v_left_end;
{
int64_t slim_v_right_start = {0};
slim_v_right_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_right, slim_allocation_region);
(void)slim_v_right_start;
{
int64_t slim_v_right_end = {0};
slim_v_right_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_right, slim_allocation_region);
(void)slim_v_right_end;
slim_result = slim_fn_project_span_less_chars(slim_v_source, slim_v_left_start, slim_v_left_end, slim_v_right_start, slim_v_right_end, slim_allocation_region);
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_find_unsorted_module(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_previous, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_previous;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
bool slim_v_first = {0};
slim_v_first = slim_v_previous < INT64_C(0);
(void)slim_v_first;
{
bool slim_v_ordered = {0};
if (slim_v_first) {
slim_v_ordered = true;
}
else {
slim_v_ordered = slim_fn_project_span_less(slim_v_source, slim_v_tokens, slim_v_previous, slim_v_name, slim_allocation_region);
}
(void)slim_v_ordered;
if (!slim_v_ordered) {
slim_result = slim_v_name;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
slim_v_previous = slim_v_name;
goto slim_recur;
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_find_prior_module_name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_current, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_current;
(void)slim_v_name_start;
(void)slim_v_name_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_reached = {0};
slim_v_reached = slim_v_cursor == slim_v_current;
(void)slim_v_reached;
if (slim_v_reached) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_spans_equal(slim_v_source, slim_v_start, slim_v_end, slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_name;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_find_duplicate_module(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_modules;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_end;
{
int64_t slim_v_prior = {0};
slim_v_prior = slim_fn_project_find_prior_module_name(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_cursor, slim_v_start, slim_v_end, slim_allocation_region);
(void)slim_v_prior;
{
bool slim_v_duplicate = {0};
slim_v_duplicate = slim_v_prior >= INT64_C(0);
(void)slim_v_duplicate;
if (slim_v_duplicate) {
slim_result = slim_v_name;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_find_prior_path(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_current, int64_t slim_v_path_start, int64_t slim_v_path_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_current;
(void)slim_v_path_start;
(void)slim_v_path_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_reached = {0};
slim_v_reached = slim_v_cursor == slim_v_current;
(void)slim_v_reached;
if (slim_v_reached) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_path = {0};
slim_v_path = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_path;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_path, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_path, slim_allocation_region);
(void)slim_v_end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_spans_equal(slim_v_source, slim_v_start, slim_v_end, slim_v_path_start, slim_v_path_end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_path;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_find_duplicate_path(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_modules;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_path = {0};
slim_v_path = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_path;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_path, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_path, slim_allocation_region);
(void)slim_v_end;
{
int64_t slim_v_prior = {0};
slim_v_prior = slim_fn_project_find_prior_path(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_cursor, slim_v_start, slim_v_end, slim_allocation_region);
(void)slim_v_prior;
{
bool slim_v_duplicate = {0};
slim_v_duplicate = slim_v_prior >= INT64_C(0);
(void)slim_v_duplicate;
if (slim_v_duplicate) {
slim_result = slim_v_path;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_project_path_segment_valid(SlimBytes slim_v_source, int64_t slim_v_start, int64_t slim_v_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_start;
(void)slim_v_end;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_length = {0};
slim_v_length = slim_i64_sub(slim_v_end, slim_v_start);
(void)slim_v_length;
{
bool slim_v_nonempty = {0};
slim_v_nonempty = slim_v_length > INT64_C(0);
(void)slim_v_nonempty;
if (!slim_v_nonempty) {
slim_result = false;
}
else {
{
bool slim_v_dot = {0};
slim_v_dot = slim_fn_syntax_span_equal(slim_v_source, slim_v_start, slim_v_end, slim_bytes_static((const uint8_t *)".", (int64_t)(sizeof(".") - 1)), slim_allocation_region);
(void)slim_v_dot;
{
bool slim_v_parent = {0};
slim_v_parent = slim_fn_syntax_span_equal(slim_v_source, slim_v_start, slim_v_end, slim_bytes_static((const uint8_t *)"..", (int64_t)(sizeof("..") - 1)), slim_allocation_region);
(void)slim_v_parent;
{
bool slim_v_special = {0};
slim_v_special = slim_v_dot || slim_v_parent;
(void)slim_v_special;
slim_result = !slim_v_special;
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_project_path_segments_valid(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, int64_t slim_v_segment_start, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_index;
(void)slim_v_end;
(void)slim_v_segment_start;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_fn_project_path_segment_valid(slim_v_source, slim_v_segment_start, slim_v_end, slim_allocation_region);
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_source, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
bool slim_v_backslash = {0};
slim_v_backslash = slim_v_code == INT64_C(92);
(void)slim_v_backslash;
if (slim_v_backslash) {
slim_result = false;
}
else {
{
bool slim_v_slash = {0};
slim_v_slash = slim_v_code == INT64_C(47);
(void)slim_v_slash;
if (slim_v_slash) {
{
bool slim_v_valid = {0};
slim_v_valid = slim_fn_project_path_segment_valid(slim_v_source, slim_v_segment_start, slim_v_index, slim_allocation_region);
(void)slim_v_valid;
if (!slim_v_valid) {
slim_result = false;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
slim_v_segment_start = slim_v_next;
goto slim_recur;
}
}
}
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_project_path_token_invalid(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_path, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_path;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_token_start = {0};
slim_v_token_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_path, slim_allocation_region);
(void)slim_v_token_start;
{
int64_t slim_v_token_end = {0};
slim_v_token_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_path, slim_allocation_region);
(void)slim_v_token_end;
{
int64_t slim_v_start = {0};
slim_v_start = slim_i64_add(slim_v_token_start, INT64_C(1));
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_i64_sub(slim_v_token_end, INT64_C(1));
(void)slim_v_end;
{
int64_t slim_v_length = {0};
slim_v_length = slim_i64_sub(slim_v_end, slim_v_start);
(void)slim_v_length;
{
bool slim_v_enough = {0};
slim_v_enough = slim_v_length >= INT64_C(5);
(void)slim_v_enough;
if (!slim_v_enough) {
slim_result = true;
}
else {
{
int64_t slim_v_suffix_start = {0};
slim_v_suffix_start = slim_i64_sub(slim_v_end, INT64_C(5));
(void)slim_v_suffix_start;
{
bool slim_v_suffix = {0};
slim_v_suffix = slim_fn_syntax_span_equal(slim_v_source, slim_v_suffix_start, slim_v_end, slim_bytes_static((const uint8_t *)".slim", (int64_t)(sizeof(".slim") - 1)), slim_allocation_region);
(void)slim_v_suffix;
if (!slim_v_suffix) {
slim_result = true;
}
else {
{
bool slim_v_segments = {0};
slim_v_segments = slim_fn_project_path_segments_valid(slim_v_source, slim_v_start, slim_v_end, slim_v_start, slim_allocation_region);
(void)slim_v_segments;
slim_result = !slim_v_segments;
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_find_invalid_path(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_path = {0};
slim_v_path = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_path;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_fn_project_path_token_invalid(slim_v_source, slim_v_tokens, slim_v_path, slim_allocation_region);
(void)slim_v_invalid;
if (slim_v_invalid) {
slim_result = slim_v_path;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_find_self_import_in(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_name_start;
(void)slim_v_name_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_spans_equal(slim_v_source, slim_v_start, slim_v_end, slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_find_self_import(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_start;
{
int64_t slim_v_name_end = {0};
slim_v_name_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_end;
{
int64_t slim_v_imports = {0};
slim_v_imports = slim_i64_add(slim_v_cursor, INT64_C(4));
(void)slim_v_imports;
{
int64_t slim_v_first = {0};
slim_v_first = slim_i64_add(slim_v_imports, INT64_C(2));
(void)slim_v_first;
{
int64_t slim_v_invalid = {0};
slim_v_invalid = slim_fn_project_find_self_import_in(slim_v_source, slim_v_tokens, slim_v_first, slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_invalid;
{
bool slim_v_found = {0};
slim_v_found = slim_v_invalid >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_invalid;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_find_unknown_import_in(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_modules;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_end;
{
int64_t slim_v_module = {0};
slim_v_module = slim_fn_project_find_manifest_module(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_start, slim_v_end, slim_allocation_region);
(void)slim_v_module;
{
bool slim_v_known = {0};
slim_v_known = slim_v_module >= INT64_C(0);
(void)slim_v_known;
if (!slim_v_known) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_find_unknown_import(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_modules;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_imports = {0};
slim_v_imports = slim_i64_add(slim_v_cursor, INT64_C(4));
(void)slim_v_imports;
{
int64_t slim_v_first = {0};
slim_v_first = slim_i64_add(slim_v_imports, INT64_C(2));
(void)slim_v_first;
{
int64_t slim_v_invalid = {0};
slim_v_invalid = slim_fn_project_find_unknown_import_in(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_first, slim_allocation_region);
(void)slim_v_invalid;
{
bool slim_v_found = {0};
slim_v_found = slim_v_invalid >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_invalid;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_report_manifest_rules(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_entry_form, int64_t slim_v_modules, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_entry_form;
(void)slim_v_modules;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_entry = {0};
slim_v_entry = slim_i64_add(slim_v_entry_form, INT64_C(2));
(void)slim_v_entry;
{
int64_t slim_v_entry_start = {0};
slim_v_entry_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_entry, slim_allocation_region);
(void)slim_v_entry_start;
{
int64_t slim_v_entry_end = {0};
slim_v_entry_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_entry, slim_allocation_region);
(void)slim_v_entry_end;
{
int64_t slim_v_declared = {0};
slim_v_declared = slim_fn_project_find_manifest_module(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_entry_start, slim_v_entry_end, slim_allocation_region);
(void)slim_v_declared;
{
bool slim_v_known_entry = {0};
slim_v_known_entry = slim_v_declared >= INT64_C(0);
(void)slim_v_known_entry;
if (!slim_v_known_entry) {
slim_result = slim_fn_project_report_project_diagnostic(slim_bytes_static((const uint8_t *)"E0403", (int64_t)(sizeof("E0403") - 1)), slim_bytes_static((const uint8_t *)"-", (int64_t)(sizeof("-") - 1)), slim_v_entry_start, slim_v_entry_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
int64_t slim_v_unsorted = {0};
slim_v_unsorted = slim_fn_project_find_unsorted_module(slim_v_source, slim_v_tokens, slim_v_modules, INT64_C(-1), slim_allocation_region);
(void)slim_v_unsorted;
{
bool slim_v_has_unsorted = {0};
slim_v_has_unsorted = slim_v_unsorted >= INT64_C(0);
(void)slim_v_has_unsorted;
if (slim_v_has_unsorted) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_unsorted, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_unsorted, slim_allocation_region);
(void)slim_v_end;
{
int64_t slim_v_order_report = {0};
slim_v_order_report = slim_fn_project_report_project_diagnostic(slim_bytes_static((const uint8_t *)"E0406", (int64_t)(sizeof("E0406") - 1)), slim_bytes_static((const uint8_t *)"-", (int64_t)(sizeof("-") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_order_report;
{
int64_t slim_v_duplicate = {0};
slim_v_duplicate = slim_fn_project_find_duplicate_module(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_modules, slim_allocation_region);
(void)slim_v_duplicate;
{
bool slim_v_same_span = {0};
slim_v_same_span = slim_v_duplicate == slim_v_unsorted;
(void)slim_v_same_span;
if (slim_v_same_span) {
{
int64_t slim_v_duplicate_report = {0};
slim_v_duplicate_report = slim_fn_project_report_project_diagnostic(slim_bytes_static((const uint8_t *)"E0408", (int64_t)(sizeof("E0408") - 1)), slim_bytes_static((const uint8_t *)"-", (int64_t)(sizeof("-") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_duplicate_report;
slim_result = slim_v_order_report;
}
}
else {
slim_result = slim_v_order_report;
}
}
}
}
}
}
}
else {
{
int64_t slim_v_duplicate_path = {0};
slim_v_duplicate_path = slim_fn_project_find_duplicate_path(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_modules, slim_allocation_region);
(void)slim_v_duplicate_path;
{
bool slim_v_has_duplicate_path = {0};
slim_v_has_duplicate_path = slim_v_duplicate_path >= INT64_C(0);
(void)slim_v_has_duplicate_path;
if (slim_v_has_duplicate_path) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_duplicate_path, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_duplicate_path, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_project_report_project_diagnostic(slim_bytes_static((const uint8_t *)"E0408", (int64_t)(sizeof("E0408") - 1)), slim_bytes_static((const uint8_t *)"-", (int64_t)(sizeof("-") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
{
int64_t slim_v_invalid_path = {0};
slim_v_invalid_path = slim_fn_project_find_invalid_path(slim_v_source, slim_v_tokens, slim_v_modules, slim_allocation_region);
(void)slim_v_invalid_path;
{
bool slim_v_has_invalid_path = {0};
slim_v_has_invalid_path = slim_v_invalid_path >= INT64_C(0);
(void)slim_v_has_invalid_path;
if (slim_v_has_invalid_path) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_invalid_path, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_invalid_path, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_project_report_project_diagnostic(slim_bytes_static((const uint8_t *)"E0407", (int64_t)(sizeof("E0407") - 1)), slim_bytes_static((const uint8_t *)"-", (int64_t)(sizeof("-") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
{
int64_t slim_v_self_import = {0};
slim_v_self_import = slim_fn_project_find_self_import(slim_v_source, slim_v_tokens, slim_v_modules, slim_allocation_region);
(void)slim_v_self_import;
{
bool slim_v_has_self = {0};
slim_v_has_self = slim_v_self_import >= INT64_C(0);
(void)slim_v_has_self;
if (slim_v_has_self) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_self_import, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_self_import, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_project_report_project_diagnostic(slim_bytes_static((const uint8_t *)"E0412", (int64_t)(sizeof("E0412") - 1)), slim_bytes_static((const uint8_t *)"-", (int64_t)(sizeof("-") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
{
int64_t slim_v_unknown_import = {0};
slim_v_unknown_import = slim_fn_project_find_unknown_import(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_modules, slim_allocation_region);
(void)slim_v_unknown_import;
{
bool slim_v_has_unknown = {0};
slim_v_has_unknown = slim_v_unknown_import >= INT64_C(0);
(void)slim_v_has_unknown;
if (slim_v_has_unknown) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_unknown_import, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_unknown_import, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_project_report_project_diagnostic(slim_bytes_static((const uint8_t *)"E0411", (int64_t)(sizeof("E0411") - 1)), slim_bytes_static((const uint8_t *)"-", (int64_t)(sizeof("-") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
slim_result = INT64_C(0);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_load_project_modules(SlimBytes slim_v_manifest_path, SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_cursor, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, SlimRegion *slim_region) {
(void)slim_v_manifest_path;
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_cursor;
(void)slim_v_module_tokens;
(void)slim_v_loaded;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_manifest_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(0);
}
else {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_path = {0};
slim_v_path = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_path;
{
SlimBytes slim_v_module_path = {0};
slim_v_module_path = slim_fn_project_project_module_path(slim_v_manifest_path, slim_v_manifest_source, slim_v_manifest_tokens, slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_path;
{
SlimVec slim_v_source_bytes = {0};
slim_v_source_bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_source_bytes;
{
bool slim_v_source_read = {0};
slim_v_source_read = slim_read_file(slim_v_module_path, &slim_v_source_bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_source_read;
if (!slim_v_source_read) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_manifest_tokens, slim_v_path, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_manifest_tokens, slim_v_path, slim_allocation_region);
(void)slim_v_end;
{
SlimBytes slim_v_module_name = {0};
slim_v_module_name = slim_fn_project_token_bytes(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_name, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_name;
slim_result = slim_fn_project_report_project_diagnostic(slim_bytes_static((const uint8_t *)"E0409", (int64_t)(sizeof("E0409") - 1)), slim_v_module_name, slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
else {
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_bytes_freeze(slim_v_source_bytes);
(void)slim_v_source;
{
int64_t slim_v_root = {0};
slim_v_root = ((*slim_v_module_tokens)).len;
(void)slim_v_root;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_lex(slim_v_source, INT64_C(0), slim_v_module_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
{
Slim_type_project_LoadedModule slim_v_module = {0};
{
int64_t slim_t_41340 = {0};
slim_t_41340 = slim_v_cursor;
int64_t slim_t_41344 = {0};
slim_t_41344 = slim_v_name;
int64_t slim_t_41348 = {0};
slim_t_41348 = slim_v_path;
SlimBytes slim_t_41352 = {0};
slim_t_41352 = slim_v_source;
int64_t slim_t_41356 = {0};
slim_t_41356 = slim_v_root;
slim_v_module = (Slim_type_project_LoadedModule){.slim_field_manifest = slim_t_41340, .slim_field_name = slim_t_41344, .slim_field_path = slim_t_41348, .slim_field_source = slim_t_41352, .slim_field_root = slim_t_41356};
}
(void)slim_v_module;
{
SlimUnit slim_v_pushed = {0};
if (!slim_vec_push(slim_v_loaded, &(slim_v_module))) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
(void)slim_v_pushed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_manifest_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_report_module_identities(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_module_tokens;
(void)slim_v_loaded;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_loaded)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(0);
}
else {
{
Slim_type_project_LoadedModule slim_v_module = {0};
slim_v_module = ((Slim_type_project_LoadedModule *)((*slim_v_loaded)).data)[slim_vec_check_index(&((*slim_v_loaded)), slim_v_index)];
(void)slim_v_module;
{
int64_t slim_v_name = {0};
slim_v_name = slim_v_module.slim_field_name;
(void)slim_v_name;
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_v_module.slim_field_source;
(void)slim_v_source;
{
int64_t slim_v_root = {0};
slim_v_root = slim_v_module.slim_field_root;
(void)slim_v_root;
{
int64_t slim_v_source_name = {0};
slim_v_source_name = slim_i64_add(slim_v_root, INT64_C(2));
(void)slim_v_source_name;
{
int64_t slim_v_manifest_start = {0};
slim_v_manifest_start = slim_fn_syntax_token_start(slim_v_manifest_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_manifest_start;
{
int64_t slim_v_manifest_end = {0};
slim_v_manifest_end = slim_fn_syntax_token_end(slim_v_manifest_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_manifest_end;
{
int64_t slim_v_source_start = {0};
slim_v_source_start = slim_fn_syntax_token_start(slim_v_module_tokens, slim_v_source_name, slim_allocation_region);
(void)slim_v_source_start;
{
int64_t slim_v_source_end = {0};
slim_v_source_end = slim_fn_syntax_token_end(slim_v_module_tokens, slim_v_source_name, slim_allocation_region);
(void)slim_v_source_end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_project_cross_spans_equal(slim_v_manifest_source, slim_v_manifest_start, slim_v_manifest_end, slim_v_source, slim_v_source_start, slim_v_source_end, slim_allocation_region);
(void)slim_v_same;
if (!slim_v_same) {
{
SlimBytes slim_v_module_name = {0};
slim_v_module_name = slim_fn_project_token_bytes(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_name, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_name;
slim_result = slim_fn_project_report_project_diagnostic(slim_bytes_static((const uint8_t *)"E0410", (int64_t)(sizeof("E0410") - 1)), slim_v_module_name, slim_v_manifest_start, slim_v_manifest_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_find_missing_export(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_cursor, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_module_items, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_cursor;
(void)slim_v_module_source;
(void)slim_v_module_tokens;
(void)slim_v_module_items;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_manifest_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_declaration = {0};
slim_v_declaration = slim_fn_project_find_export_declaration(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_cursor, slim_v_module_source, slim_v_module_tokens, slim_v_module_items, slim_allocation_region);
(void)slim_v_declaration;
{
bool slim_v_found = {0};
slim_v_found = slim_v_declaration >= INT64_C(0);
(void)slim_v_found;
if (!slim_v_found) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_report_missing_exports(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_module_tokens;
(void)slim_v_loaded;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_loaded)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(0);
}
else {
{
Slim_type_project_LoadedModule slim_v_module = {0};
slim_v_module = ((Slim_type_project_LoadedModule *)((*slim_v_loaded)).data)[slim_vec_check_index(&((*slim_v_loaded)), slim_v_index)];
(void)slim_v_module;
{
int64_t slim_v_manifest = {0};
slim_v_manifest = slim_v_module.slim_field_manifest;
(void)slim_v_manifest;
{
int64_t slim_v_name = {0};
slim_v_name = slim_v_module.slim_field_name;
(void)slim_v_name;
{
int64_t slim_v_imports = {0};
slim_v_imports = slim_i64_add(slim_v_manifest, INT64_C(4));
(void)slim_v_imports;
{
int64_t slim_v_exports = {0};
slim_v_exports = slim_fn_syntax_skip_form(slim_v_manifest_tokens, slim_v_imports, slim_allocation_region);
(void)slim_v_exports;
{
int64_t slim_v_first_export = {0};
slim_v_first_export = slim_i64_add(slim_v_exports, INT64_C(2));
(void)slim_v_first_export;
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_v_module.slim_field_source;
(void)slim_v_source;
{
int64_t slim_v_root = {0};
slim_v_root = slim_v_module.slim_field_root;
(void)slim_v_root;
{
int64_t slim_v_module_items = {0};
slim_v_module_items = slim_i64_add(slim_v_root, INT64_C(3));
(void)slim_v_module_items;
{
int64_t slim_v_missing = {0};
slim_v_missing = slim_fn_project_find_missing_export(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_first_export, slim_v_source, slim_v_module_tokens, slim_v_module_items, slim_allocation_region);
(void)slim_v_missing;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_missing >= INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_manifest_tokens, slim_v_missing, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_manifest_tokens, slim_v_missing, slim_allocation_region);
(void)slim_v_end;
{
SlimBytes slim_v_module_name = {0};
slim_v_module_name = slim_fn_project_token_bytes(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_name, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_name;
slim_result = slim_fn_project_report_project_diagnostic(slim_bytes_static((const uint8_t *)"E0414", (int64_t)(sizeof("E0414") - 1)), slim_v_module_name, slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_find_named_declaration(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimBytes slim_v_name, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_name;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_declaration_name = {0};
slim_v_declaration_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_declaration_name;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_declaration_name, slim_v_name, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_project_loaded_is_entry(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_entry, Slim_type_project_LoadedModule slim_v_module, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_entry;
(void)slim_v_module;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_name = {0};
slim_v_name = slim_v_module.slim_field_name;
(void)slim_v_name;
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_fn_syntax_token_start(slim_v_manifest_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_start;
{
int64_t slim_v_name_end = {0};
slim_v_name_end = slim_fn_syntax_token_end(slim_v_manifest_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_end;
{
int64_t slim_v_entry_start = {0};
slim_v_entry_start = slim_fn_syntax_token_start(slim_v_manifest_tokens, slim_v_entry, slim_allocation_region);
(void)slim_v_entry_start;
{
int64_t slim_v_entry_end = {0};
slim_v_entry_end = slim_fn_syntax_token_end(slim_v_manifest_tokens, slim_v_entry, slim_allocation_region);
(void)slim_v_entry_end;
slim_result = slim_fn_syntax_spans_equal(slim_v_manifest_source, slim_v_name_start, slim_v_name_end, slim_v_entry_start, slim_v_entry_end, slim_allocation_region);
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_report_project_mains(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_entry, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_entry;
(void)slim_v_module_tokens;
(void)slim_v_loaded;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_loaded)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(0);
}
else {
{
Slim_type_project_LoadedModule slim_v_module = {0};
slim_v_module = ((Slim_type_project_LoadedModule *)((*slim_v_loaded)).data)[slim_vec_check_index(&((*slim_v_loaded)), slim_v_index)];
(void)slim_v_module;
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_v_module.slim_field_source;
(void)slim_v_source;
{
int64_t slim_v_root = {0};
slim_v_root = slim_v_module.slim_field_root;
(void)slim_v_root;
{
int64_t slim_v_module_items = {0};
slim_v_module_items = slim_i64_add(slim_v_root, INT64_C(3));
(void)slim_v_module_items;
{
int64_t slim_v_main = {0};
slim_v_main = slim_fn_project_find_named_declaration(slim_v_source, slim_v_module_tokens, slim_v_module_items, slim_bytes_static((const uint8_t *)"main", (int64_t)(sizeof("main") - 1)), slim_allocation_region);
(void)slim_v_main;
{
bool slim_v_has_main = {0};
slim_v_has_main = slim_v_main >= INT64_C(0);
(void)slim_v_has_main;
{
bool slim_v_is_entry = {0};
slim_v_is_entry = slim_fn_project_loaded_is_entry(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_entry, slim_v_module, slim_allocation_region);
(void)slim_v_is_entry;
if (slim_v_is_entry) {
if (slim_v_has_main) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
else {
{
int64_t slim_v_name = {0};
slim_v_name = slim_v_module.slim_field_name;
(void)slim_v_name;
{
SlimBytes slim_v_module_name = {0};
slim_v_module_name = slim_fn_project_token_bytes(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_name, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_name;
{
int64_t slim_v_next_form = {0};
slim_v_next_form = slim_fn_syntax_skip_form(slim_v_module_tokens, slim_v_root, slim_allocation_region);
(void)slim_v_next_form;
{
int64_t slim_v_closing = {0};
slim_v_closing = slim_i64_sub(slim_v_next_form, INT64_C(1));
(void)slim_v_closing;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_module_tokens, slim_v_closing, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_project_report_project_diagnostic(slim_bytes_static((const uint8_t *)"E0419", (int64_t)(sizeof("E0419") - 1)), slim_v_module_name, INT64_C(0), slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
else {
if (!slim_v_has_main) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
else {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_module_tokens, slim_v_main, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_next_form = {0};
slim_v_next_form = slim_fn_syntax_skip_form(slim_v_module_tokens, slim_v_main, slim_allocation_region);
(void)slim_v_next_form;
{
int64_t slim_v_closing = {0};
slim_v_closing = slim_i64_sub(slim_v_next_form, INT64_C(1));
(void)slim_v_closing;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_module_tokens, slim_v_closing, slim_allocation_region);
(void)slim_v_end;
{
int64_t slim_v_name = {0};
slim_v_name = slim_v_module.slim_field_name;
(void)slim_v_name;
{
SlimBytes slim_v_module_name = {0};
slim_v_module_name = slim_fn_project_token_bytes(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_name, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_name;
slim_result = slim_fn_project_report_project_diagnostic(slim_bytes_static((const uint8_t *)"E0419", (int64_t)(sizeof("E0419") - 1)), slim_v_module_name, slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_find_loaded_segment(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_loaded, SlimBytes slim_v_reference_source, int64_t slim_v_segment_start, int64_t slim_v_segment_end, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_loaded;
(void)slim_v_reference_source;
(void)slim_v_segment_start;
(void)slim_v_segment_end;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_loaded)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
Slim_type_project_LoadedModule slim_v_module = {0};
slim_v_module = ((Slim_type_project_LoadedModule *)((*slim_v_loaded)).data)[slim_vec_check_index(&((*slim_v_loaded)), slim_v_index)];
(void)slim_v_module;
{
int64_t slim_v_name = {0};
slim_v_name = slim_v_module.slim_field_name;
(void)slim_v_name;
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_fn_syntax_token_start(slim_v_manifest_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_start;
{
int64_t slim_v_name_end = {0};
slim_v_name_end = slim_fn_syntax_token_end(slim_v_manifest_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_project_cross_spans_equal(slim_v_manifest_source, slim_v_name_start, slim_v_name_end, slim_v_reference_source, slim_v_segment_start, slim_v_segment_end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_index;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_find_declaration_segment(SlimBytes slim_v_declaration_source, SlimVec * slim_v_module_tokens, int64_t slim_v_cursor, SlimBytes slim_v_reference_source, int64_t slim_v_segment_start, int64_t slim_v_segment_end, SlimRegion *slim_region) {
(void)slim_v_declaration_source;
(void)slim_v_module_tokens;
(void)slim_v_cursor;
(void)slim_v_reference_source;
(void)slim_v_segment_start;
(void)slim_v_segment_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_fn_syntax_token_start(slim_v_module_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_start;
{
int64_t slim_v_name_end = {0};
slim_v_name_end = slim_fn_syntax_token_end(slim_v_module_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_project_cross_spans_equal(slim_v_declaration_source, slim_v_name_start, slim_v_name_end, slim_v_reference_source, slim_v_segment_start, slim_v_segment_end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_target_call_status(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, Slim_type_project_LoadedModule slim_v_current, int64_t slim_v_target_index, SlimBytes slim_v_reference_source, int64_t slim_v_prefix_start, int64_t slim_v_slash, int64_t slim_v_reference_end, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_module_tokens;
(void)slim_v_loaded;
(void)slim_v_current;
(void)slim_v_target_index;
(void)slim_v_reference_source;
(void)slim_v_prefix_start;
(void)slim_v_slash;
(void)slim_v_reference_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
int64_t slim_v_current_manifest = {0};
slim_v_current_manifest = slim_v_current.slim_field_manifest;
(void)slim_v_current_manifest;
{
int64_t slim_v_imports = {0};
slim_v_imports = slim_i64_add(slim_v_current_manifest, INT64_C(4));
(void)slim_v_imports;
{
int64_t slim_v_first_import = {0};
slim_v_first_import = slim_i64_add(slim_v_imports, INT64_C(2));
(void)slim_v_first_import;
{
bool slim_v_imported = {0};
slim_v_imported = slim_fn_project_exports_has_segment(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_first_import, slim_v_reference_source, slim_v_prefix_start, slim_v_slash, slim_allocation_region);
(void)slim_v_imported;
if (!slim_v_imported) {
slim_result = INT64_C(2);
}
else {
{
Slim_type_project_LoadedModule slim_v_target = {0};
slim_v_target = ((Slim_type_project_LoadedModule *)((*slim_v_loaded)).data)[slim_vec_check_index(&((*slim_v_loaded)), slim_v_target_index)];
(void)slim_v_target;
{
int64_t slim_v_target_manifest = {0};
slim_v_target_manifest = slim_v_target.slim_field_manifest;
(void)slim_v_target_manifest;
{
int64_t slim_v_target_imports = {0};
slim_v_target_imports = slim_i64_add(slim_v_target_manifest, INT64_C(4));
(void)slim_v_target_imports;
{
int64_t slim_v_exports = {0};
slim_v_exports = slim_fn_syntax_skip_form(slim_v_manifest_tokens, slim_v_target_imports, slim_allocation_region);
(void)slim_v_exports;
{
int64_t slim_v_first_export = {0};
slim_v_first_export = slim_i64_add(slim_v_exports, INT64_C(2));
(void)slim_v_first_export;
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_i64_add(slim_v_slash, INT64_C(1));
(void)slim_v_name_start;
{
bool slim_v_exported = {0};
slim_v_exported = slim_fn_project_exports_has_segment(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_first_export, slim_v_reference_source, slim_v_name_start, slim_v_reference_end, slim_allocation_region);
(void)slim_v_exported;
if (!slim_v_exported) {
slim_result = INT64_C(3);
}
else {
{
SlimBytes slim_v_target_source = {0};
slim_v_target_source = slim_v_target.slim_field_source;
(void)slim_v_target_source;
{
int64_t slim_v_target_root = {0};
slim_v_target_root = slim_v_target.slim_field_root;
(void)slim_v_target_root;
{
int64_t slim_v_target_items = {0};
slim_v_target_items = slim_i64_add(slim_v_target_root, INT64_C(3));
(void)slim_v_target_items;
{
int64_t slim_v_declaration = {0};
slim_v_declaration = slim_fn_project_find_declaration_segment(slim_v_target_source, slim_v_module_tokens, slim_v_target_items, slim_v_reference_source, slim_v_name_start, slim_v_reference_end, slim_allocation_region);
(void)slim_v_declaration;
{
bool slim_v_found = {0};
slim_v_found = slim_v_declaration >= INT64_C(0);
(void)slim_v_found;
{
bool slim_v_callable = {0};
if (!slim_v_found) {
slim_v_callable = false;
}
else {
slim_v_callable = slim_fn_syntax_item_is(slim_v_target_source, slim_v_module_tokens, slim_v_declaration, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
}
(void)slim_v_callable;
if (slim_v_callable) {
slim_result = INT64_C(0);
}
else {
slim_result = INT64_C(3);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_qualified_call_status(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, Slim_type_project_LoadedModule slim_v_current, int64_t slim_v_callee, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_module_tokens;
(void)slim_v_loaded;
(void)slim_v_current;
(void)slim_v_callee;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
SlimBytes slim_v_reference_source = {0};
slim_v_reference_source = slim_v_current.slim_field_source;
(void)slim_v_reference_source;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_module_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_module_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_end;
{
int64_t slim_v_slash = {0};
slim_v_slash = slim_fn_project_find_span_slash(slim_v_reference_source, slim_v_start, slim_v_end, slim_allocation_region);
(void)slim_v_slash;
{
bool slim_v_qualified = {0};
slim_v_qualified = slim_v_slash >= INT64_C(0);
(void)slim_v_qualified;
if (!slim_v_qualified) {
slim_result = INT64_C(0);
}
else {
{
int64_t slim_v_current_name = {0};
slim_v_current_name = slim_v_current.slim_field_name;
(void)slim_v_current_name;
{
int64_t slim_v_current_start = {0};
slim_v_current_start = slim_fn_syntax_token_start(slim_v_manifest_tokens, slim_v_current_name, slim_allocation_region);
(void)slim_v_current_start;
{
int64_t slim_v_current_end = {0};
slim_v_current_end = slim_fn_syntax_token_end(slim_v_manifest_tokens, slim_v_current_name, slim_allocation_region);
(void)slim_v_current_end;
{
bool slim_v_self = {0};
slim_v_self = slim_fn_project_cross_spans_equal(slim_v_manifest_source, slim_v_current_start, slim_v_current_end, slim_v_reference_source, slim_v_start, slim_v_slash, slim_allocation_region);
(void)slim_v_self;
if (slim_v_self) {
slim_result = INT64_C(1);
}
else {
{
int64_t slim_v_target = {0};
slim_v_target = slim_fn_project_find_loaded_segment(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_loaded, slim_v_reference_source, slim_v_start, slim_v_slash, INT64_C(0), slim_allocation_region);
(void)slim_v_target;
{
bool slim_v_found = {0};
slim_v_found = slim_v_target >= INT64_C(0);
(void)slim_v_found;
if (!slim_v_found) {
slim_result = INT64_C(2);
}
else {
slim_result = slim_fn_project_target_call_status(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_tokens, slim_v_loaded, slim_v_current, slim_v_target, slim_v_reference_source, slim_v_start, slim_v_slash, slim_v_end, slim_allocation_region);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_find_invalid_loaded_call(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, Slim_type_project_LoadedModule slim_v_current, int64_t slim_v_cursor, int64_t slim_v_end, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_module_tokens;
(void)slim_v_loaded;
(void)slim_v_current;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_call_form = {0};
if (slim_v_form) {
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_v_current.slim_field_source;
(void)slim_v_source;
slim_v_call_form = slim_fn_syntax_token_equal(slim_v_source, slim_v_module_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"call", (int64_t)(sizeof("call") - 1)), slim_allocation_region);
}
}
else {
slim_v_call_form = false;
}
(void)slim_v_call_form;
{
int64_t slim_v_callee = {0};
slim_v_callee = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_callee;
{
int64_t slim_v_status = {0};
if (slim_v_call_form) {
slim_v_status = slim_fn_project_qualified_call_status(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_tokens, slim_v_loaded, slim_v_current, slim_v_callee, slim_allocation_region);
}
else {
slim_v_status = INT64_C(0);
}
(void)slim_v_status;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_status > INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_report_qualified_calls(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_module_tokens;
(void)slim_v_loaded;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_loaded)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(0);
}
else {
{
Slim_type_project_LoadedModule slim_v_module = {0};
slim_v_module = ((Slim_type_project_LoadedModule *)((*slim_v_loaded)).data)[slim_vec_check_index(&((*slim_v_loaded)), slim_v_index)];
(void)slim_v_module;
{
int64_t slim_v_root = {0};
slim_v_root = slim_v_module.slim_field_root;
(void)slim_v_root;
{
int64_t slim_v_first = {0};
slim_v_first = slim_i64_add(slim_v_root, INT64_C(3));
(void)slim_v_first;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_skip_form(slim_v_module_tokens, slim_v_root, slim_allocation_region);
(void)slim_v_end;
{
int64_t slim_v_invalid = {0};
slim_v_invalid = slim_fn_project_find_invalid_loaded_call(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_tokens, slim_v_loaded, slim_v_module, slim_v_first, slim_v_end, slim_allocation_region);
(void)slim_v_invalid;
{
bool slim_v_found = {0};
slim_v_found = slim_v_invalid >= INT64_C(0);
(void)slim_v_found;
if (!slim_v_found) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
else {
{
int64_t slim_v_callee = {0};
slim_v_callee = slim_i64_add(slim_v_invalid, INT64_C(2));
(void)slim_v_callee;
{
int64_t slim_v_status = {0};
slim_v_status = slim_fn_project_qualified_call_status(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_tokens, slim_v_loaded, slim_v_module, slim_v_callee, slim_allocation_region);
(void)slim_v_status;
{
bool slim_v_self = {0};
slim_v_self = slim_v_status == INT64_C(1);
(void)slim_v_self;
{
SlimBytes slim_v_code = {0};
if (slim_v_self) {
slim_v_code = slim_bytes_static((const uint8_t *)"E0417", (int64_t)(sizeof("E0417") - 1));
}
else {
{
bool slim_v_unimported = {0};
slim_v_unimported = slim_v_status == INT64_C(2);
(void)slim_v_unimported;
if (slim_v_unimported) {
slim_v_code = slim_bytes_static((const uint8_t *)"E0416", (int64_t)(sizeof("E0416") - 1));
}
else {
slim_v_code = slim_bytes_static((const uint8_t *)"E0415", (int64_t)(sizeof("E0415") - 1));
}
}
}
(void)slim_v_code;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_module_tokens, slim_v_invalid, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_next_form = {0};
slim_v_next_form = slim_fn_syntax_skip_form(slim_v_module_tokens, slim_v_invalid, slim_allocation_region);
(void)slim_v_next_form;
{
int64_t slim_v_closing = {0};
slim_v_closing = slim_i64_sub(slim_v_next_form, INT64_C(1));
(void)slim_v_closing;
{
int64_t slim_v_call_end = {0};
slim_v_call_end = slim_fn_syntax_token_end(slim_v_module_tokens, slim_v_closing, slim_allocation_region);
(void)slim_v_call_end;
{
int64_t slim_v_name = {0};
slim_v_name = slim_v_module.slim_field_name;
(void)slim_v_name;
{
SlimBytes slim_v_module_name = {0};
slim_v_module_name = slim_fn_project_token_bytes(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_name, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_name;
slim_result = slim_fn_project_report_project_diagnostic(slim_v_code, slim_v_module_name, slim_v_start, slim_v_call_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_report_loaded_project(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_entry, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_entry;
(void)slim_v_module_tokens;
(void)slim_v_loaded;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_identities = {0};
slim_v_identities = slim_fn_project_report_module_identities(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_tokens, slim_v_loaded, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_identities;
{
bool slim_v_invalid_identity = {0};
slim_v_invalid_identity = slim_v_identities > INT64_C(0);
(void)slim_v_invalid_identity;
if (slim_v_invalid_identity) {
slim_result = slim_v_identities;
}
else {
{
int64_t slim_v_exports = {0};
slim_v_exports = slim_fn_project_report_missing_exports(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_tokens, slim_v_loaded, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_exports;
{
bool slim_v_invalid_export = {0};
slim_v_invalid_export = slim_v_exports > INT64_C(0);
(void)slim_v_invalid_export;
if (slim_v_invalid_export) {
slim_result = slim_v_exports;
}
else {
{
int64_t slim_v_mains = {0};
slim_v_mains = slim_fn_project_report_project_mains(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_entry, slim_v_module_tokens, slim_v_loaded, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_mains;
{
bool slim_v_invalid_main = {0};
slim_v_invalid_main = slim_v_mains > INT64_C(0);
(void)slim_v_invalid_main;
if (slim_v_invalid_main) {
slim_result = slim_v_mains;
}
else {
{
int64_t slim_v_calls = {0};
slim_v_calls = slim_fn_project_report_qualified_calls(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_tokens, slim_v_loaded, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_calls;
{
bool slim_v_invalid_call = {0};
slim_v_invalid_call = slim_v_calls > INT64_C(0);
(void)slim_v_invalid_call;
if (slim_v_invalid_call) {
slim_result = slim_v_calls;
}
else {
slim_result = slim_fn_project_report_private_type_leaks(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_tokens, slim_v_loaded, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_report_interface_type(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_first_export, SlimBytes slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_module_items, int64_t slim_v_type_index, int64_t slim_v_diagnostic_start, int64_t slim_v_diagnostic_end, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_first_export;
(void)slim_v_module_name;
(void)slim_v_module_source;
(void)slim_v_module_tokens;
(void)slim_v_module_items;
(void)slim_v_type_index;
(void)slim_v_diagnostic_start;
(void)slim_v_diagnostic_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_module_tokens, slim_v_type_index, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_composite = {0};
slim_v_composite = slim_v_kind == INT64_C(0);
(void)slim_v_composite;
if (slim_v_composite) {
{
int64_t slim_v_inner = {0};
slim_v_inner = slim_i64_add(slim_v_type_index, INT64_C(2));
(void)slim_v_inner;
slim_result = slim_fn_project_report_interface_type(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_first_export, slim_v_module_name, slim_v_module_source, slim_v_module_tokens, slim_v_module_items, slim_v_inner, slim_v_diagnostic_start, slim_v_diagnostic_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
else {
{
int64_t slim_v_scalar_kind = {0};
slim_v_scalar_kind = slim_fn_syntax_scalar_type_kind(slim_v_module_source, slim_v_module_tokens, slim_v_type_index, slim_allocation_region);
(void)slim_v_scalar_kind;
{
bool slim_v_scalar = {0};
slim_v_scalar = slim_v_scalar_kind >= INT64_C(0);
(void)slim_v_scalar;
if (slim_v_scalar) {
slim_result = INT64_C(0);
}
else {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_module_tokens, slim_v_type_index, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_module_tokens, slim_v_type_index, slim_allocation_region);
(void)slim_v_end;
{
int64_t slim_v_slashes = {0};
slim_v_slashes = slim_fn_syntax_span_count_byte(slim_v_module_source, slim_v_start, slim_v_end, INT64_C(47), INT64_C(0), slim_allocation_region);
(void)slim_v_slashes;
{
bool slim_v_qualified = {0};
slim_v_qualified = slim_v_slashes > INT64_C(0);
(void)slim_v_qualified;
if (slim_v_qualified) {
slim_result = INT64_C(0);
}
else {
{
int64_t slim_v_declaration = {0};
slim_v_declaration = slim_fn_project_find_declaration_segment(slim_v_module_source, slim_v_module_tokens, slim_v_module_items, slim_v_module_source, slim_v_start, slim_v_end, slim_allocation_region);
(void)slim_v_declaration;
{
bool slim_v_found = {0};
slim_v_found = slim_v_declaration >= INT64_C(0);
(void)slim_v_found;
{
bool slim_v_record_item = {0};
if (!slim_v_found) {
slim_v_record_item = false;
}
else {
slim_v_record_item = slim_fn_syntax_item_is(slim_v_module_source, slim_v_module_tokens, slim_v_declaration, slim_bytes_static((const uint8_t *)"record", (int64_t)(sizeof("record") - 1)), slim_allocation_region);
}
(void)slim_v_record_item;
{
bool slim_v_variant_item = {0};
if (!slim_v_found) {
slim_v_variant_item = false;
}
else {
slim_v_variant_item = slim_fn_syntax_item_is(slim_v_module_source, slim_v_module_tokens, slim_v_declaration, slim_bytes_static((const uint8_t *)"variant", (int64_t)(sizeof("variant") - 1)), slim_allocation_region);
}
(void)slim_v_variant_item;
{
bool slim_v_type_item = {0};
slim_v_type_item = slim_v_record_item || slim_v_variant_item;
(void)slim_v_type_item;
{
bool slim_v_exported = {0};
slim_v_exported = slim_fn_project_exports_has_segment(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_first_export, slim_v_module_source, slim_v_start, slim_v_end, slim_allocation_region);
(void)slim_v_exported;
{
bool slim_v_valid = {0};
slim_v_valid = slim_v_type_item && slim_v_exported;
(void)slim_v_valid;
if (slim_v_valid) {
slim_result = INT64_C(0);
}
else {
slim_result = slim_fn_project_report_project_diagnostic(slim_bytes_static((const uint8_t *)"E0418", (int64_t)(sizeof("E0418") - 1)), slim_v_module_name, slim_v_diagnostic_start, slim_v_diagnostic_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_report_interface_bindings(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_first_export, SlimBytes slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_module_items, int64_t slim_v_cursor, int64_t slim_v_diagnostic_start, int64_t slim_v_diagnostic_end, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_first_export;
(void)slim_v_module_name;
(void)slim_v_module_source;
(void)slim_v_module_tokens;
(void)slim_v_module_items;
(void)slim_v_cursor;
(void)slim_v_diagnostic_start;
(void)slim_v_diagnostic_end;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_token_equal(slim_v_module_source, slim_v_module_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"inout", (int64_t)(sizeof("inout") - 1)), slim_allocation_region);
(void)slim_v_borrowed;
{
int64_t slim_v_offset = {0};
if (slim_v_borrowed) {
slim_v_offset = INT64_C(3);
}
else {
slim_v_offset = INT64_C(2);
}
(void)slim_v_offset;
{
int64_t slim_v_type_index = {0};
slim_v_type_index = slim_i64_add(slim_v_cursor, slim_v_offset);
(void)slim_v_type_index;
{
int64_t slim_v_reported = {0};
slim_v_reported = slim_fn_project_report_interface_type(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_first_export, slim_v_module_name, slim_v_module_source, slim_v_module_tokens, slim_v_module_items, slim_v_type_index, slim_v_diagnostic_start, slim_v_diagnostic_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_reported;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
slim_v_cursor = slim_v_next;
slim_v_count = slim_v_total;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_report_interface_payload(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_first_export, SlimBytes slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_module_items, int64_t slim_v_cursor, int64_t slim_v_diagnostic_start, int64_t slim_v_diagnostic_end, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_first_export;
(void)slim_v_module_name;
(void)slim_v_module_source;
(void)slim_v_module_tokens;
(void)slim_v_module_items;
(void)slim_v_cursor;
(void)slim_v_diagnostic_start;
(void)slim_v_diagnostic_end;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
int64_t slim_v_reported = {0};
slim_v_reported = slim_fn_project_report_interface_type(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_first_export, slim_v_module_name, slim_v_module_source, slim_v_module_tokens, slim_v_module_items, slim_v_cursor, slim_v_diagnostic_start, slim_v_diagnostic_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_reported;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
slim_v_cursor = slim_v_next;
slim_v_count = slim_v_total;
goto slim_recur;
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_report_interface_cases(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_first_export, SlimBytes slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_module_items, int64_t slim_v_cursor, int64_t slim_v_diagnostic_start, int64_t slim_v_diagnostic_end, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_first_export;
(void)slim_v_module_name;
(void)slim_v_module_source;
(void)slim_v_module_tokens;
(void)slim_v_module_items;
(void)slim_v_cursor;
(void)slim_v_diagnostic_start;
(void)slim_v_diagnostic_end;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
int64_t slim_v_first_type = {0};
slim_v_first_type = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_first_type;
{
int64_t slim_v_reported = {0};
slim_v_reported = slim_fn_project_report_interface_payload(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_first_export, slim_v_module_name, slim_v_module_source, slim_v_module_tokens, slim_v_module_items, slim_v_first_type, slim_v_diagnostic_start, slim_v_diagnostic_end, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_reported;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
slim_v_cursor = slim_v_next;
slim_v_count = slim_v_total;
goto slim_recur;
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_report_export_interfaces(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, int64_t slim_v_first_export, SlimBytes slim_v_module_name, SlimBytes slim_v_module_source, SlimVec * slim_v_module_tokens, int64_t slim_v_module_items, int64_t slim_v_cursor, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_first_export;
(void)slim_v_module_name;
(void)slim_v_module_source;
(void)slim_v_module_tokens;
(void)slim_v_module_items;
(void)slim_v_cursor;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_manifest_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
int64_t slim_v_item = {0};
slim_v_item = slim_fn_project_find_export_declaration(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_cursor, slim_v_module_source, slim_v_module_tokens, slim_v_module_items, slim_allocation_region);
(void)slim_v_item;
{
int64_t slim_v_diagnostic_start = {0};
slim_v_diagnostic_start = slim_fn_syntax_token_start(slim_v_manifest_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_diagnostic_start;
{
int64_t slim_v_diagnostic_end = {0};
slim_v_diagnostic_end = slim_fn_syntax_token_end(slim_v_manifest_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_diagnostic_end;
{
bool slim_v_function_item = {0};
slim_v_function_item = slim_fn_syntax_item_is(slim_v_module_source, slim_v_module_tokens, slim_v_item, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_item;
{
int64_t slim_v_reported = {0};
if (slim_v_function_item) {
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_item, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_first_parameter = {0};
slim_v_first_parameter = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_first_parameter;
{
int64_t slim_v_parameter_errors = {0};
slim_v_parameter_errors = slim_fn_project_report_interface_bindings(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_first_export, slim_v_module_name, slim_v_module_source, slim_v_module_tokens, slim_v_module_items, slim_v_first_parameter, slim_v_diagnostic_start, slim_v_diagnostic_end, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_parameter_errors;
{
int64_t slim_v_return_type = {0};
slim_v_return_type = slim_fn_syntax_skip_form(slim_v_module_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_type;
{
int64_t slim_v_return_errors = {0};
slim_v_return_errors = slim_fn_project_report_interface_type(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_first_export, slim_v_module_name, slim_v_module_source, slim_v_module_tokens, slim_v_module_items, slim_v_return_type, slim_v_diagnostic_start, slim_v_diagnostic_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_return_errors;
slim_v_reported = slim_i64_add(slim_v_parameter_errors, slim_v_return_errors);
}
}
}
}
}
}
else {
{
bool slim_v_record_item = {0};
slim_v_record_item = slim_fn_syntax_item_is(slim_v_module_source, slim_v_module_tokens, slim_v_item, slim_bytes_static((const uint8_t *)"record", (int64_t)(sizeof("record") - 1)), slim_allocation_region);
(void)slim_v_record_item;
{
int64_t slim_v_body = {0};
slim_v_body = slim_i64_add(slim_v_item, INT64_C(3));
(void)slim_v_body;
{
int64_t slim_v_first_member = {0};
slim_v_first_member = slim_i64_add(slim_v_body, INT64_C(1));
(void)slim_v_first_member;
if (slim_v_record_item) {
slim_v_reported = slim_fn_project_report_interface_bindings(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_first_export, slim_v_module_name, slim_v_module_source, slim_v_module_tokens, slim_v_module_items, slim_v_first_member, slim_v_diagnostic_start, slim_v_diagnostic_end, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_reported = slim_fn_project_report_interface_cases(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_first_export, slim_v_module_name, slim_v_module_source, slim_v_module_tokens, slim_v_module_items, slim_v_first_member, slim_v_diagnostic_start, slim_v_diagnostic_end, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
(void)slim_v_reported;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
slim_v_cursor = slim_v_next;
slim_v_count = slim_v_total;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_report_private_type_leaks_from(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, int64_t slim_v_index, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_module_tokens;
(void)slim_v_loaded;
(void)slim_v_index;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_loaded)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
Slim_type_project_LoadedModule slim_v_module = {0};
slim_v_module = ((Slim_type_project_LoadedModule *)((*slim_v_loaded)).data)[slim_vec_check_index(&((*slim_v_loaded)), slim_v_index)];
(void)slim_v_module;
{
int64_t slim_v_manifest = {0};
slim_v_manifest = slim_v_module.slim_field_manifest;
(void)slim_v_manifest;
{
int64_t slim_v_name = {0};
slim_v_name = slim_v_module.slim_field_name;
(void)slim_v_name;
{
SlimBytes slim_v_module_name = {0};
slim_v_module_name = slim_fn_project_token_bytes(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_name, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_name;
{
int64_t slim_v_imports = {0};
slim_v_imports = slim_i64_add(slim_v_manifest, INT64_C(4));
(void)slim_v_imports;
{
int64_t slim_v_exports = {0};
slim_v_exports = slim_fn_syntax_skip_form(slim_v_manifest_tokens, slim_v_imports, slim_allocation_region);
(void)slim_v_exports;
{
int64_t slim_v_first_export = {0};
slim_v_first_export = slim_i64_add(slim_v_exports, INT64_C(2));
(void)slim_v_first_export;
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_v_module.slim_field_source;
(void)slim_v_source;
{
int64_t slim_v_root = {0};
slim_v_root = slim_v_module.slim_field_root;
(void)slim_v_root;
{
int64_t slim_v_module_items = {0};
slim_v_module_items = slim_i64_add(slim_v_root, INT64_C(3));
(void)slim_v_module_items;
{
int64_t slim_v_reported = {0};
slim_v_reported = slim_fn_project_report_export_interfaces(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_first_export, slim_v_module_name, slim_v_source, slim_v_module_tokens, slim_v_module_items, slim_v_first_export, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_reported;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
slim_v_index = slim_v_next;
slim_v_count = slim_v_total;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_report_private_type_leaks(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_module_tokens;
(void)slim_v_loaded;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_errors = {0};
slim_v_errors = slim_fn_project_report_private_type_leaks_from(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_tokens, slim_v_loaded, INT64_C(0), INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_errors;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_errors > INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
slim_result = INT64_C(1);
}
else {
slim_result = INT64_C(0);
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimBytes slim_fn_query_copy_span(SlimBytes slim_v_source, int64_t slim_v_start, int64_t slim_v_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_start;
(void)slim_v_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimBytes slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_appended = {0};
slim_v_appended = slim_fn_text_append_span(slim_v_source, slim_v_start, slim_v_end, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_appended;
slim_result = slim_bytes_freeze(slim_v_output);
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_query_bytes_equal_from(SlimBytes slim_v_left, SlimBytes slim_v_right, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region) {
(void)slim_v_left;
(void)slim_v_right;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = true;
}
else {
{
uint8_t slim_v_left_byte = {0};
slim_v_left_byte = slim_bytes_get(slim_v_left, slim_v_index);
(void)slim_v_left_byte;
{
uint8_t slim_v_right_byte = {0};
slim_v_right_byte = slim_bytes_get(slim_v_right, slim_v_index);
(void)slim_v_right_byte;
{
int64_t slim_v_left_code = {0};
slim_v_left_code = (int64_t)slim_v_left_byte;
(void)slim_v_left_code;
{
int64_t slim_v_right_code = {0};
slim_v_right_code = (int64_t)slim_v_right_byte;
(void)slim_v_right_code;
{
bool slim_v_same = {0};
slim_v_same = slim_v_left_code == slim_v_right_code;
(void)slim_v_same;
if (!slim_v_same) {
slim_result = false;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_query_bytes_equal(SlimBytes slim_v_left, SlimBytes slim_v_right, SlimRegion *slim_region) {
(void)slim_v_left;
(void)slim_v_right;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_left_length = {0};
slim_v_left_length = slim_bytes_len(slim_v_left);
(void)slim_v_left_length;
{
int64_t slim_v_right_length = {0};
slim_v_right_length = slim_bytes_len(slim_v_right);
(void)slim_v_right_length;
{
bool slim_v_same_length = {0};
slim_v_same_length = slim_v_left_length == slim_v_right_length;
(void)slim_v_same_length;
if (!slim_v_same_length) {
slim_result = false;
}
else {
slim_result = slim_fn_query_bytes_equal_from(slim_v_left, slim_v_right, INT64_C(0), slim_v_left_length, slim_allocation_region);
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_query_declaration_kind(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_item, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_item;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_function_item = {0};
slim_v_function_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_item, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_item;
if (slim_v_function_item) {
slim_result = INT64_C(0);
}
else {
{
bool slim_v_record_item = {0};
slim_v_record_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_item, slim_bytes_static((const uint8_t *)"record", (int64_t)(sizeof("record") - 1)), slim_allocation_region);
(void)slim_v_record_item;
if (slim_v_record_item) {
slim_result = INT64_C(1);
}
else {
slim_result = INT64_C(2);
}
}
}
}
return slim_result;
}

static int64_t slim_fn_query_declaration_interface_end(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_item, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_item;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_function_item = {0};
slim_v_function_item = slim_fn_syntax_item_is(slim_v_source, slim_v_tokens, slim_v_item, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_item;
if (!slim_v_function_item) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_item, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_closing = {0};
slim_v_closing = slim_i64_sub(slim_v_next, INT64_C(1));
(void)slim_v_closing;
slim_result = slim_fn_syntax_token_end(slim_v_tokens, slim_v_closing, slim_allocation_region);
}
}
}
else {
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_item, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_type = {0};
slim_v_return_type = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_type;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_return_type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_after_effects = {0};
slim_v_after_effects = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_effects, slim_allocation_region);
(void)slim_v_after_effects;
{
int64_t slim_v_closing = {0};
slim_v_closing = slim_i64_sub(slim_v_after_effects, INT64_C(1));
(void)slim_v_closing;
slim_result = slim_fn_syntax_token_end(slim_v_tokens, slim_v_closing, slim_allocation_region);
}
}
}
}
}
}
}
return slim_result;
}

static SlimUnit slim_fn_query_append_module_snapshots(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_module_tokens, Slim_type_project_LoadedModule slim_v_module, int64_t slim_v_cursor, SlimBytes slim_v_module_name, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_module_tokens;
(void)slim_v_module;
(void)slim_v_cursor;
(void)slim_v_module_name;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_v_module.slim_field_source;
(void)slim_v_source;
{
int64_t slim_v_name_token = {0};
slim_v_name_token = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name_token;
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_fn_syntax_token_start(slim_v_module_tokens, slim_v_name_token, slim_allocation_region);
(void)slim_v_name_start;
{
int64_t slim_v_name_end = {0};
slim_v_name_end = slim_fn_syntax_token_end(slim_v_module_tokens, slim_v_name_token, slim_allocation_region);
(void)slim_v_name_end;
{
SlimBytes slim_v_name = {0};
slim_v_name = slim_fn_query_copy_span(slim_v_source, slim_v_name_start, slim_v_name_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_name;
{
int64_t slim_v_query_declaration_kind = {0};
slim_v_query_declaration_kind = slim_fn_query_declaration_kind(slim_v_source, slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_query_declaration_kind;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_interface_end = {0};
slim_v_interface_end = slim_fn_query_declaration_interface_end(slim_v_source, slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_interface_end;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_module_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_closing = {0};
slim_v_closing = slim_i64_sub(slim_v_next, INT64_C(1));
(void)slim_v_closing;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_module_tokens, slim_v_closing, slim_allocation_region);
(void)slim_v_end;
{
Slim_type_query_Snapshot slim_v_snapshot = {0};
{
SlimBytes slim_t_46504 = {0};
slim_t_46504 = slim_v_module_name;
int64_t slim_t_46508 = {0};
slim_t_46508 = slim_v_query_declaration_kind;
SlimBytes slim_t_46512 = {0};
slim_t_46512 = slim_v_name;
SlimBytes slim_t_46516 = {0};
slim_t_46516 = slim_v_source;
int64_t slim_t_46520 = {0};
slim_t_46520 = slim_v_cursor;
int64_t slim_t_46524 = {0};
slim_t_46524 = slim_v_next;
int64_t slim_t_46528 = {0};
slim_t_46528 = slim_v_start;
int64_t slim_t_46532 = {0};
slim_t_46532 = slim_v_interface_end;
int64_t slim_t_46536 = {0};
slim_t_46536 = slim_v_end;
slim_v_snapshot = (Slim_type_query_Snapshot){.slim_field_module = slim_t_46504, .slim_field_kind = slim_t_46508, .slim_field_name = slim_t_46512, .slim_field_source = slim_t_46516, .slim_field_form = slim_t_46520, .slim_field_next = slim_t_46524, .slim_field_start = slim_t_46528, .slim_field_interface_end = slim_t_46532, .slim_field_end = slim_t_46536};
}
(void)slim_v_snapshot;
{
SlimUnit slim_v_pushed = {0};
if (!slim_vec_push(slim_v_output, &(slim_v_snapshot))) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
(void)slim_v_pushed;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_query_build_snapshots_from(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_module_tokens;
(void)slim_v_loaded;
(void)slim_v_index;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_loaded)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
Slim_type_project_LoadedModule slim_v_module = {0};
slim_v_module = ((Slim_type_project_LoadedModule *)((*slim_v_loaded)).data)[slim_vec_check_index(&((*slim_v_loaded)), slim_v_index)];
(void)slim_v_module;
{
int64_t slim_v_manifest_name = {0};
slim_v_manifest_name = slim_v_module.slim_field_name;
(void)slim_v_manifest_name;
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_fn_syntax_token_start(slim_v_manifest_tokens, slim_v_manifest_name, slim_allocation_region);
(void)slim_v_name_start;
{
int64_t slim_v_name_end = {0};
slim_v_name_end = slim_fn_syntax_token_end(slim_v_manifest_tokens, slim_v_manifest_name, slim_allocation_region);
(void)slim_v_name_end;
{
SlimBytes slim_v_module_name = {0};
slim_v_module_name = slim_fn_query_copy_span(slim_v_manifest_source, slim_v_name_start, slim_v_name_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_name;
{
int64_t slim_v_root = {0};
slim_v_root = slim_v_module.slim_field_root;
(void)slim_v_root;
{
int64_t slim_v_first = {0};
slim_v_first = slim_i64_add(slim_v_root, INT64_C(3));
(void)slim_v_first;
{
SlimUnit slim_v_appended = {0};
slim_v_appended = slim_fn_query_append_module_snapshots(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_tokens, slim_v_module, slim_v_first, slim_v_module_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_appended;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimVec slim_fn_query_build_snapshots(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_module_tokens;
(void)slim_v_loaded;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimVec slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(Slim_type_query_Snapshot), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_built = {0};
slim_v_built = slim_fn_query_build_snapshots_from(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_tokens, slim_v_loaded, INT64_C(0), &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_built;
slim_result = slim_v_output;
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_query_snapshot_identity_equal(Slim_type_query_Snapshot slim_v_left, Slim_type_query_Snapshot slim_v_right, SlimRegion *slim_region) {
(void)slim_v_left;
(void)slim_v_right;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
SlimBytes slim_v_left_module = {0};
slim_v_left_module = slim_v_left.slim_field_module;
(void)slim_v_left_module;
{
SlimBytes slim_v_right_module = {0};
slim_v_right_module = slim_v_right.slim_field_module;
(void)slim_v_right_module;
{
bool slim_v_same_module = {0};
slim_v_same_module = slim_fn_query_bytes_equal(slim_v_left_module, slim_v_right_module, slim_allocation_region);
(void)slim_v_same_module;
{
SlimBytes slim_v_left_name = {0};
slim_v_left_name = slim_v_left.slim_field_name;
(void)slim_v_left_name;
{
SlimBytes slim_v_right_name = {0};
slim_v_right_name = slim_v_right.slim_field_name;
(void)slim_v_right_name;
{
bool slim_v_same_name = {0};
slim_v_same_name = slim_fn_query_bytes_equal(slim_v_left_name, slim_v_right_name, slim_allocation_region);
(void)slim_v_same_name;
{
int64_t slim_v_left_kind = {0};
slim_v_left_kind = slim_v_left.slim_field_kind;
(void)slim_v_left_kind;
{
int64_t slim_v_right_kind = {0};
slim_v_right_kind = slim_v_right.slim_field_kind;
(void)slim_v_right_kind;
{
bool slim_v_same_kind = {0};
slim_v_same_kind = slim_v_left_kind == slim_v_right_kind;
(void)slim_v_same_kind;
{
bool slim_v_first = {0};
slim_v_first = slim_v_same_module && slim_v_same_name;
(void)slim_v_first;
slim_result = slim_v_first && slim_v_same_kind;
}
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_query_snapshot_body_equal(Slim_type_query_Snapshot slim_v_left, Slim_type_query_Snapshot slim_v_right, SlimRegion *slim_region) {
(void)slim_v_left;
(void)slim_v_right;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_left_end = {0};
slim_v_left_end = slim_v_left.slim_field_end;
(void)slim_v_left_end;
{
int64_t slim_v_left_start = {0};
slim_v_left_start = slim_v_left.slim_field_start;
(void)slim_v_left_start;
{
int64_t slim_v_left_length = {0};
slim_v_left_length = slim_i64_sub(slim_v_left_end, slim_v_left_start);
(void)slim_v_left_length;
{
int64_t slim_v_right_end = {0};
slim_v_right_end = slim_v_right.slim_field_end;
(void)slim_v_right_end;
{
int64_t slim_v_right_start = {0};
slim_v_right_start = slim_v_right.slim_field_start;
(void)slim_v_right_start;
{
int64_t slim_v_right_length = {0};
slim_v_right_length = slim_i64_sub(slim_v_right_end, slim_v_right_start);
(void)slim_v_right_length;
{
bool slim_v_same_length = {0};
slim_v_same_length = slim_v_left_length == slim_v_right_length;
(void)slim_v_same_length;
if (!slim_v_same_length) {
slim_result = false;
}
else {
{
SlimBytes slim_v_left_bytes = {0};
slim_v_left_bytes = slim_v_left.slim_field_source;
(void)slim_v_left_bytes;
{
SlimBytes slim_v_right_bytes = {0};
slim_v_right_bytes = slim_v_right.slim_field_source;
(void)slim_v_right_bytes;
slim_result = slim_fn_project_cross_span_chars_equal(slim_v_left_bytes, slim_v_left_start, slim_v_right_bytes, slim_v_right_start, INT64_C(0), slim_v_left_length, slim_allocation_region);
}
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_query_find_snapshot(SlimVec * slim_v_snapshots, Slim_type_query_Snapshot slim_v_candidate, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_snapshots;
(void)slim_v_candidate;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_snapshots)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
Slim_type_query_Snapshot slim_v_snapshot = {0};
slim_v_snapshot = ((Slim_type_query_Snapshot *)((*slim_v_snapshots)).data)[slim_vec_check_index(&((*slim_v_snapshots)), slim_v_index)];
(void)slim_v_snapshot;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_query_snapshot_identity_equal(slim_v_snapshot, slim_v_candidate, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_index;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
return slim_result;
}

static SlimBytes slim_fn_query_snapshot_qualified_name(Slim_type_query_Snapshot slim_v_snapshot, SlimRegion *slim_region) {
(void)slim_v_snapshot;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimBytes slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimBytes slim_v_module = {0};
slim_v_module = slim_v_snapshot.slim_field_module;
(void)slim_v_module;
{
SlimUnit slim_v_module_text = {0};
slim_v_module_text = slim_fn_text_append_text(&slim_v_output, slim_v_module, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_text;
{
SlimUnit slim_v_slash = {0};
slim_v_slash = slim_fn_text_append_text(&slim_v_output, slim_bytes_static((const uint8_t *)"/", (int64_t)(sizeof("/") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_slash;
{
SlimBytes slim_v_name = {0};
slim_v_name = slim_v_snapshot.slim_field_name;
(void)slim_v_name;
{
SlimUnit slim_v_name_text = {0};
slim_v_name_text = slim_fn_text_append_text(&slim_v_output, slim_v_name, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_name_text;
slim_result = slim_bytes_freeze(slim_v_output);
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_query_find_provider(SlimBytes slim_v_atom, SlimVec * slim_v_snapshots, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_atom;
(void)slim_v_snapshots;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_snapshots)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
Slim_type_query_Snapshot slim_v_snapshot = {0};
slim_v_snapshot = ((Slim_type_query_Snapshot *)((*slim_v_snapshots)).data)[slim_vec_check_index(&((*slim_v_snapshots)), slim_v_index)];
(void)slim_v_snapshot;
{
SlimBytes slim_v_qualified = {0};
slim_v_qualified = slim_fn_query_snapshot_qualified_name(slim_v_snapshot, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_qualified;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_query_bytes_equal(slim_v_atom, slim_v_qualified, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_index;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_query_dependency_exists(SlimVec * slim_v_dependencies, int64_t slim_v_consumer, int64_t slim_v_provider, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_dependencies;
(void)slim_v_consumer;
(void)slim_v_provider;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_dependencies)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
Slim_type_query_Dependency slim_v_dependency = {0};
slim_v_dependency = ((Slim_type_query_Dependency *)((*slim_v_dependencies)).data)[slim_vec_check_index(&((*slim_v_dependencies)), slim_v_index)];
(void)slim_v_dependency;
{
int64_t slim_v_candidate_consumer = {0};
slim_v_candidate_consumer = slim_v_dependency.slim_field_consumer;
(void)slim_v_candidate_consumer;
{
bool slim_v_same_consumer = {0};
slim_v_same_consumer = slim_v_candidate_consumer == slim_v_consumer;
(void)slim_v_same_consumer;
{
int64_t slim_v_candidate_provider = {0};
slim_v_candidate_provider = slim_v_dependency.slim_field_provider;
(void)slim_v_candidate_provider;
{
bool slim_v_same_provider = {0};
slim_v_same_provider = slim_v_candidate_provider == slim_v_provider;
(void)slim_v_same_provider;
{
bool slim_v_same = {0};
slim_v_same = slim_v_same_consumer && slim_v_same_provider;
(void)slim_v_same;
if (slim_v_same) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static SlimUnit slim_fn_query_append_snapshot_dependencies(SlimVec * slim_v_tokens, SlimVec * slim_v_snapshots, int64_t slim_v_consumer, int64_t slim_v_cursor, int64_t slim_v_end, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_snapshots;
(void)slim_v_consumer;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
Slim_type_query_Snapshot slim_v_snapshot = {0};
slim_v_snapshot = ((Slim_type_query_Snapshot *)((*slim_v_snapshots)).data)[slim_vec_check_index(&((*slim_v_snapshots)), slim_v_consumer)];
(void)slim_v_snapshot;
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_v_snapshot.slim_field_source;
(void)slim_v_source;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_atom_kind = {0};
slim_v_atom_kind = slim_v_kind == INT64_C(2);
(void)slim_v_atom_kind;
{
int64_t slim_v_provider = {0};
if (slim_v_atom_kind) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_finish = {0};
slim_v_finish = slim_fn_syntax_token_end(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_finish;
{
SlimBytes slim_v_atom = {0};
slim_v_atom = slim_fn_query_copy_span(slim_v_source, slim_v_start, slim_v_finish, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_atom;
slim_v_provider = slim_fn_query_find_provider(slim_v_atom, slim_v_snapshots, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
else {
slim_v_provider = INT64_C(-1);
}
(void)slim_v_provider;
{
bool slim_v_found = {0};
slim_v_found = slim_v_provider >= INT64_C(0);
(void)slim_v_found;
{
bool slim_v_self = {0};
slim_v_self = slim_v_consumer == slim_v_provider;
(void)slim_v_self;
{
bool slim_v_not_self = {0};
slim_v_not_self = !slim_v_self;
(void)slim_v_not_self;
{
bool slim_v_cross = {0};
slim_v_cross = slim_v_found && slim_v_not_self;
(void)slim_v_cross;
{
bool slim_v_duplicate = {0};
if (slim_v_cross) {
slim_v_duplicate = slim_fn_query_dependency_exists(slim_v_output, slim_v_consumer, slim_v_provider, INT64_C(0), slim_allocation_region);
}
else {
slim_v_duplicate = false;
}
(void)slim_v_duplicate;
{
bool slim_v_unique = {0};
slim_v_unique = !slim_v_duplicate;
(void)slim_v_unique;
{
bool slim_v_append = {0};
slim_v_append = slim_v_cross && slim_v_unique;
(void)slim_v_append;
{
SlimUnit slim_v_pushed = {0};
if (slim_v_append) {
{
Slim_type_query_Dependency slim_v_dependency = {0};
{
int64_t slim_t_47837 = {0};
slim_t_47837 = slim_v_consumer;
int64_t slim_t_47841 = {0};
slim_t_47841 = slim_v_provider;
slim_v_dependency = (Slim_type_query_Dependency){.slim_field_consumer = slim_t_47837, .slim_field_provider = slim_t_47841};
}
(void)slim_v_dependency;
if (!slim_vec_push(slim_v_output, &(slim_v_dependency))) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
}
}
else {
slim_v_pushed = (SlimUnit){0};
}
(void)slim_v_pushed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_query_build_dependencies_from(SlimVec * slim_v_tokens, SlimVec * slim_v_snapshots, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_snapshots;
(void)slim_v_index;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_snapshots)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
Slim_type_query_Snapshot slim_v_snapshot = {0};
slim_v_snapshot = ((Slim_type_query_Snapshot *)((*slim_v_snapshots)).data)[slim_vec_check_index(&((*slim_v_snapshots)), slim_v_index)];
(void)slim_v_snapshot;
{
int64_t slim_v_form = {0};
slim_v_form = slim_v_snapshot.slim_field_form;
(void)slim_v_form;
{
int64_t slim_v_next_form = {0};
slim_v_next_form = slim_v_snapshot.slim_field_next;
(void)slim_v_next_form;
{
SlimUnit slim_v_appended = {0};
slim_v_appended = slim_fn_query_append_snapshot_dependencies(slim_v_tokens, slim_v_snapshots, slim_v_index, slim_v_form, slim_v_next_form, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_appended;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimVec slim_fn_query_build_dependencies(SlimVec * slim_v_tokens, SlimVec * slim_v_snapshots, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_snapshots;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimVec slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(Slim_type_query_Dependency), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_built = {0};
slim_v_built = slim_fn_query_build_dependencies_from(slim_v_tokens, slim_v_snapshots, INT64_C(0), &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_built;
slim_result = slim_v_output;
}
}
slim_allocation_failed: ;
return slim_result;
}

static Slim_type_query_State slim_fn_query_build_state(SlimBytes slim_v_manifest_source, SlimVec * slim_v_manifest_tokens, SlimVec * slim_v_module_tokens, SlimVec * slim_v_loaded, SlimRegion *slim_region) {
(void)slim_v_manifest_source;
(void)slim_v_manifest_tokens;
(void)slim_v_module_tokens;
(void)slim_v_loaded;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
Slim_type_query_State slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_snapshots = {0};
slim_v_snapshots = slim_fn_query_build_snapshots(slim_v_manifest_source, slim_v_manifest_tokens, slim_v_module_tokens, slim_v_loaded, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_snapshots;
{
SlimVec slim_v_dependencies = {0};
slim_v_dependencies = slim_fn_query_build_dependencies(slim_v_module_tokens, &slim_v_snapshots, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dependencies;
{
SlimVec slim_t_48159 = {0};
slim_t_48159 = slim_v_snapshots;
SlimVec slim_t_48163 = {0};
slim_t_48163 = slim_v_dependencies;
slim_result = (Slim_type_query_State){.slim_field_snapshots = slim_t_48159, .slim_field_dependencies = slim_t_48163};
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_query_snapshot_interface_equal(Slim_type_query_Snapshot slim_v_left, Slim_type_query_Snapshot slim_v_right, SlimRegion *slim_region) {
(void)slim_v_left;
(void)slim_v_right;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_left_end = {0};
slim_v_left_end = slim_v_left.slim_field_interface_end;
(void)slim_v_left_end;
{
int64_t slim_v_left_start = {0};
slim_v_left_start = slim_v_left.slim_field_start;
(void)slim_v_left_start;
{
int64_t slim_v_left_length = {0};
slim_v_left_length = slim_i64_sub(slim_v_left_end, slim_v_left_start);
(void)slim_v_left_length;
{
int64_t slim_v_right_end = {0};
slim_v_right_end = slim_v_right.slim_field_interface_end;
(void)slim_v_right_end;
{
int64_t slim_v_right_start = {0};
slim_v_right_start = slim_v_right.slim_field_start;
(void)slim_v_right_start;
{
int64_t slim_v_right_length = {0};
slim_v_right_length = slim_i64_sub(slim_v_right_end, slim_v_right_start);
(void)slim_v_right_length;
{
bool slim_v_same_length = {0};
slim_v_same_length = slim_v_left_length == slim_v_right_length;
(void)slim_v_same_length;
if (!slim_v_same_length) {
slim_result = false;
}
else {
{
SlimBytes slim_v_left_bytes = {0};
slim_v_left_bytes = slim_v_left.slim_field_source;
(void)slim_v_left_bytes;
{
SlimBytes slim_v_right_bytes = {0};
slim_v_right_bytes = slim_v_right.slim_field_source;
(void)slim_v_right_bytes;
slim_result = slim_fn_project_cross_span_chars_equal(slim_v_left_bytes, slim_v_left_start, slim_v_right_bytes, slim_v_right_start, INT64_C(0), slim_v_left_length, slim_allocation_region);
}
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static SlimUnit slim_fn_query_fill_flags(int64_t slim_v_length, int64_t slim_v_index, SlimVec * slim_v_flags, SlimRegion *slim_region) {
(void)slim_v_length;
(void)slim_v_index;
(void)slim_v_flags;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
bool slim_v_initial = {0};
slim_v_initial = false;
(void)slim_v_initial;
{
SlimUnit slim_v_pushed = {0};
if (!slim_vec_push(slim_v_flags, &(slim_v_initial))) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
(void)slim_v_pushed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_query_classify_changes(SlimVec * slim_v_previous, SlimVec * slim_v_current, int64_t slim_v_index, int64_t slim_v_count, SlimVec * slim_v_invalidated, SlimVec * slim_v_queue, SlimRegion *slim_region) {
(void)slim_v_previous;
(void)slim_v_current;
(void)slim_v_index;
(void)slim_v_count;
(void)slim_v_invalidated;
(void)slim_v_queue;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_current)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
Slim_type_query_Snapshot slim_v_candidate = {0};
slim_v_candidate = ((Slim_type_query_Snapshot *)((*slim_v_current)).data)[slim_vec_check_index(&((*slim_v_current)), slim_v_index)];
(void)slim_v_candidate;
{
int64_t slim_v_prior = {0};
slim_v_prior = slim_fn_query_find_snapshot(slim_v_previous, slim_v_candidate, INT64_C(0), slim_allocation_region);
(void)slim_v_prior;
{
bool slim_v_found = {0};
slim_v_found = slim_v_prior >= INT64_C(0);
(void)slim_v_found;
{
bool slim_v_body_changed = {0};
if (!slim_v_found) {
slim_v_body_changed = true;
}
else {
{
Slim_type_query_Snapshot slim_v_old = {0};
slim_v_old = ((Slim_type_query_Snapshot *)((*slim_v_previous)).data)[slim_vec_check_index(&((*slim_v_previous)), slim_v_prior)];
(void)slim_v_old;
{
bool slim_v_same_body = {0};
slim_v_same_body = slim_fn_query_snapshot_body_equal(slim_v_old, slim_v_candidate, slim_allocation_region);
(void)slim_v_same_body;
slim_v_body_changed = !slim_v_same_body;
}
}
}
(void)slim_v_body_changed;
{
bool slim_v_interface_changed = {0};
if (!slim_v_found) {
slim_v_interface_changed = true;
}
else {
{
Slim_type_query_Snapshot slim_v_old = {0};
slim_v_old = ((Slim_type_query_Snapshot *)((*slim_v_previous)).data)[slim_vec_check_index(&((*slim_v_previous)), slim_v_prior)];
(void)slim_v_old;
{
bool slim_v_same_interface = {0};
slim_v_same_interface = slim_fn_query_snapshot_interface_equal(slim_v_old, slim_v_candidate, slim_allocation_region);
(void)slim_v_same_interface;
slim_v_interface_changed = !slim_v_same_interface;
}
}
}
(void)slim_v_interface_changed;
{
SlimUnit slim_v_marked = {0};
if (slim_v_body_changed) {
{
bool slim_v_value = {0};
slim_v_value = true;
(void)slim_v_value;
slim_vec_set(slim_v_invalidated, slim_v_index, &slim_v_value); slim_v_marked = (SlimUnit){0};
}
}
else {
slim_v_marked = (SlimUnit){0};
}
(void)slim_v_marked;
{
SlimUnit slim_v_enqueued = {0};
if (slim_v_interface_changed) {
if (!slim_vec_push(slim_v_queue, &(slim_v_index))) goto slim_allocation_failed; slim_v_enqueued = (SlimUnit){0};
}
else {
slim_v_enqueued = (SlimUnit){0};
}
(void)slim_v_enqueued;
{
int64_t slim_v_increment = {0};
if (slim_v_body_changed) {
slim_v_increment = INT64_C(1);
}
else {
slim_v_increment = INT64_C(0);
}
(void)slim_v_increment;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_increment);
(void)slim_v_total;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
slim_v_count = slim_v_total;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_query_mark_consumers(SlimVec * slim_v_dependencies, int64_t slim_v_provider, int64_t slim_v_index, SlimVec * slim_v_invalidated, SlimVec * slim_v_queue, SlimRegion *slim_region) {
(void)slim_v_dependencies;
(void)slim_v_provider;
(void)slim_v_index;
(void)slim_v_invalidated;
(void)slim_v_queue;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_dependencies)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
Slim_type_query_Dependency slim_v_dependency = {0};
slim_v_dependency = ((Slim_type_query_Dependency *)((*slim_v_dependencies)).data)[slim_vec_check_index(&((*slim_v_dependencies)), slim_v_index)];
(void)slim_v_dependency;
{
int64_t slim_v_candidate_provider = {0};
slim_v_candidate_provider = slim_v_dependency.slim_field_provider;
(void)slim_v_candidate_provider;
{
bool slim_v_same = {0};
slim_v_same = slim_v_candidate_provider == slim_v_provider;
(void)slim_v_same;
{
int64_t slim_v_consumer = {0};
slim_v_consumer = slim_v_dependency.slim_field_consumer;
(void)slim_v_consumer;
{
bool slim_v_already = {0};
slim_v_already = ((bool *)((*slim_v_invalidated)).data)[slim_vec_check_index(&((*slim_v_invalidated)), slim_v_consumer)];
(void)slim_v_already;
{
bool slim_v_fresh = {0};
slim_v_fresh = !slim_v_already;
(void)slim_v_fresh;
{
bool slim_v_mark = {0};
slim_v_mark = slim_v_same && slim_v_fresh;
(void)slim_v_mark;
{
SlimUnit slim_v_marked = {0};
if (slim_v_mark) {
{
bool slim_v_value = {0};
slim_v_value = true;
(void)slim_v_value;
slim_vec_set(slim_v_invalidated, slim_v_consumer, &slim_v_value); slim_v_marked = (SlimUnit){0};
}
}
else {
slim_v_marked = (SlimUnit){0};
}
(void)slim_v_marked;
{
SlimUnit slim_v_enqueued = {0};
if (slim_v_mark) {
if (!slim_vec_push(slim_v_queue, &(slim_v_consumer))) goto slim_allocation_failed; slim_v_enqueued = (SlimUnit){0};
}
else {
slim_v_enqueued = (SlimUnit){0};
}
(void)slim_v_enqueued;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_query_propagate_invalidations(SlimVec * slim_v_dependencies, SlimVec * slim_v_queue, int64_t slim_v_cursor, SlimVec * slim_v_invalidated, SlimRegion *slim_region) {
(void)slim_v_dependencies;
(void)slim_v_queue;
(void)slim_v_cursor;
(void)slim_v_invalidated;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_queue)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_provider = {0};
slim_v_provider = ((int64_t *)((*slim_v_queue)).data)[slim_vec_check_index(&((*slim_v_queue)), slim_v_cursor)];
(void)slim_v_provider;
{
SlimUnit slim_v_marked = {0};
slim_v_marked = slim_fn_query_mark_consumers(slim_v_dependencies, slim_v_provider, INT64_C(0), slim_v_invalidated, slim_v_queue, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_marked;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_query_count_flags(SlimVec * slim_v_flags, int64_t slim_v_index, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_flags;
(void)slim_v_index;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_flags)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
bool slim_v_flag = {0};
slim_v_flag = ((bool *)((*slim_v_flags)).data)[slim_vec_check_index(&((*slim_v_flags)), slim_v_index)];
(void)slim_v_flag;
{
int64_t slim_v_increment = {0};
if (slim_v_flag) {
slim_v_increment = INT64_C(1);
}
else {
slim_v_increment = INT64_C(0);
}
(void)slim_v_increment;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_increment);
(void)slim_v_total;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
slim_v_count = slim_v_total;
goto slim_recur;
}
}
}
}
}
}
}
return slim_result;
}

static Slim_type_query_Work slim_fn_query_measure_update(SlimVec * slim_v_previous, SlimVec * slim_v_current, SlimVec * slim_v_dependencies, SlimRegion *slim_region) {
(void)slim_v_previous;
(void)slim_v_current;
(void)slim_v_dependencies;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
Slim_type_query_Work slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_current)).len;
(void)slim_v_length;
{
SlimVec slim_v_invalidated = {0};
slim_v_invalidated = slim_vec_new(sizeof(bool), slim_allocation_region);
(void)slim_v_invalidated;
{
SlimUnit slim_v_initialized = {0};
slim_v_initialized = slim_fn_query_fill_flags(slim_v_length, INT64_C(0), &slim_v_invalidated, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_initialized;
{
SlimVec slim_v_queue = {0};
slim_v_queue = slim_vec_new(sizeof(int64_t), slim_allocation_region);
(void)slim_v_queue;
{
int64_t slim_v_parsed = {0};
slim_v_parsed = slim_fn_query_classify_changes(slim_v_previous, slim_v_current, INT64_C(0), INT64_C(0), &slim_v_invalidated, &slim_v_queue, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_parsed;
{
SlimUnit slim_v_propagated = {0};
slim_v_propagated = slim_fn_query_propagate_invalidations(slim_v_dependencies, &slim_v_queue, INT64_C(0), &slim_v_invalidated, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_propagated;
{
int64_t slim_v_checked = {0};
slim_v_checked = slim_fn_query_count_flags(&slim_v_invalidated, INT64_C(0), INT64_C(0), slim_allocation_region);
(void)slim_v_checked;
{
int64_t slim_t_49258 = {0};
slim_t_49258 = slim_v_parsed;
int64_t slim_t_49262 = {0};
slim_t_49262 = slim_v_parsed;
int64_t slim_t_49266 = {0};
slim_t_49266 = slim_v_checked;
int64_t slim_t_49270 = {0};
slim_t_49270 = slim_v_checked;
slim_result = (Slim_type_query_Work){.slim_field_parsed = slim_t_49258, .slim_field_lowered = slim_t_49262, .slim_field_checked = slim_t_49266, .slim_field_generated = slim_t_49270};
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_scheduler_collect_modules(SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_modules, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_modules;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
SlimUnit slim_v_pushed = {0};
if (!slim_vec_push(slim_v_modules, &(slim_v_cursor))) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
(void)slim_v_pushed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_scheduler_fill_pending(int64_t slim_v_length, int64_t slim_v_index, SlimVec * slim_v_scheduled, SlimRegion *slim_region) {
(void)slim_v_length;
(void)slim_v_index;
(void)slim_v_scheduled;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
bool slim_v_value = {0};
slim_v_value = false;
(void)slim_v_value;
{
SlimUnit slim_v_pushed = {0};
if (!slim_vec_push(slim_v_scheduled, &(slim_v_value))) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
(void)slim_v_pushed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_scheduler_find_module(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_modules, int64_t slim_v_name, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_modules;
(void)slim_v_name;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_modules)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_module = {0};
slim_v_module = ((int64_t *)((*slim_v_modules)).data)[slim_vec_check_index(&((*slim_v_modules)), slim_v_index)];
(void)slim_v_module;
{
int64_t slim_v_candidate = {0};
slim_v_candidate = slim_i64_add(slim_v_module, INT64_C(2));
(void)slim_v_candidate;
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_start;
{
int64_t slim_v_name_end = {0};
slim_v_name_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_end;
{
int64_t slim_v_candidate_start = {0};
slim_v_candidate_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_candidate, slim_allocation_region);
(void)slim_v_candidate_start;
{
int64_t slim_v_candidate_end = {0};
slim_v_candidate_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_candidate, slim_allocation_region);
(void)slim_v_candidate_end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_spans_equal(slim_v_source, slim_v_name_start, slim_v_name_end, slim_v_candidate_start, slim_v_candidate_end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_index;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_scheduler_imports_ready(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_modules, SlimVec * slim_v_scheduled, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_modules;
(void)slim_v_scheduled;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = true;
}
else {
{
int64_t slim_v_dependency = {0};
slim_v_dependency = slim_fn_scheduler_find_module(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_cursor, INT64_C(0), slim_allocation_region);
(void)slim_v_dependency;
{
bool slim_v_found = {0};
slim_v_found = slim_v_dependency >= INT64_C(0);
(void)slim_v_found;
if (!slim_v_found) {
slim_result = false;
}
else {
{
bool slim_v_ready = {0};
slim_v_ready = ((bool *)((*slim_v_scheduled)).data)[slim_vec_check_index(&((*slim_v_scheduled)), slim_v_dependency)];
(void)slim_v_ready;
if (!slim_v_ready) {
slim_result = false;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static SlimUnit slim_fn_scheduler_collect_ready(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_modules, SlimVec * slim_v_scheduled, int64_t slim_v_index, SlimVec * slim_v_ready, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_modules;
(void)slim_v_scheduled;
(void)slim_v_index;
(void)slim_v_ready;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_modules)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
bool slim_v_complete = {0};
slim_v_complete = ((bool *)((*slim_v_scheduled)).data)[slim_vec_check_index(&((*slim_v_scheduled)), slim_v_index)];
(void)slim_v_complete;
{
bool slim_v_pending = {0};
slim_v_pending = !slim_v_complete;
(void)slim_v_pending;
{
int64_t slim_v_module = {0};
slim_v_module = ((int64_t *)((*slim_v_modules)).data)[slim_vec_check_index(&((*slim_v_modules)), slim_v_index)];
(void)slim_v_module;
{
int64_t slim_v_imports = {0};
slim_v_imports = slim_i64_add(slim_v_module, INT64_C(4));
(void)slim_v_imports;
{
int64_t slim_v_first_import = {0};
slim_v_first_import = slim_i64_add(slim_v_imports, INT64_C(2));
(void)slim_v_first_import;
{
bool slim_v_dependencies_ready = {0};
slim_v_dependencies_ready = slim_fn_scheduler_imports_ready(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_scheduled, slim_v_first_import, slim_allocation_region);
(void)slim_v_dependencies_ready;
{
bool slim_v_selectable = {0};
slim_v_selectable = slim_v_pending && slim_v_dependencies_ready;
(void)slim_v_selectable;
{
SlimUnit slim_v_pushed = {0};
if (slim_v_selectable) {
if (!slim_vec_push(slim_v_ready, &(slim_v_index))) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
}
else {
slim_v_pushed = (SlimUnit){0};
}
(void)slim_v_pushed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_scheduler_append_tasks(SlimVec * slim_v_ready, int64_t slim_v_index, int64_t slim_v_layer, SlimVec * slim_v_tasks, SlimRegion *slim_region) {
(void)slim_v_ready;
(void)slim_v_index;
(void)slim_v_layer;
(void)slim_v_tasks;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_ready)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_module = {0};
slim_v_module = ((int64_t *)((*slim_v_ready)).data)[slim_vec_check_index(&((*slim_v_ready)), slim_v_index)];
(void)slim_v_module;
{
Slim_type_scheduler_Task slim_v_task = {0};
{
int64_t slim_t_50135 = {0};
slim_t_50135 = slim_v_module;
int64_t slim_t_50139 = {0};
slim_t_50139 = slim_v_layer;
slim_v_task = (Slim_type_scheduler_Task){.slim_field_module = slim_t_50135, .slim_field_layer = slim_t_50139};
}
(void)slim_v_task;
{
SlimUnit slim_v_pushed = {0};
if (!slim_vec_push(slim_v_tasks, &(slim_v_task))) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
(void)slim_v_pushed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_scheduler_append_batches(int64_t slim_v_start, int64_t slim_v_end, int64_t slim_v_workers, SlimVec * slim_v_batches, SlimRegion *slim_region) {
(void)slim_v_start;
(void)slim_v_end;
(void)slim_v_workers;
(void)slim_v_batches;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_start >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_candidate_end = {0};
slim_v_candidate_end = slim_i64_add(slim_v_start, slim_v_workers);
(void)slim_v_candidate_end;
{
bool slim_v_exceeds = {0};
slim_v_exceeds = slim_v_candidate_end > slim_v_end;
(void)slim_v_exceeds;
{
int64_t slim_v_batch_end = {0};
if (slim_v_exceeds) {
slim_v_batch_end = slim_v_end;
}
else {
slim_v_batch_end = slim_v_candidate_end;
}
(void)slim_v_batch_end;
{
Slim_type_scheduler_Batch slim_v_batch = {0};
{
int64_t slim_t_50273 = {0};
slim_t_50273 = slim_v_start;
int64_t slim_t_50277 = {0};
slim_t_50277 = slim_v_batch_end;
slim_v_batch = (Slim_type_scheduler_Batch){.slim_field_start = slim_t_50273, .slim_field_end = slim_t_50277};
}
(void)slim_v_batch;
{
SlimUnit slim_v_pushed = {0};
if (!slim_vec_push(slim_v_batches, &(slim_v_batch))) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
(void)slim_v_pushed;
slim_v_start = slim_v_batch_end;
goto slim_recur;
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_scheduler_mark_ready(SlimVec * slim_v_ready, int64_t slim_v_index, SlimVec * slim_v_scheduled, SlimRegion *slim_region) {
(void)slim_v_ready;
(void)slim_v_index;
(void)slim_v_scheduled;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_ready)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_module = {0};
slim_v_module = ((int64_t *)((*slim_v_ready)).data)[slim_vec_check_index(&((*slim_v_ready)), slim_v_index)];
(void)slim_v_module;
{
bool slim_v_value = {0};
slim_v_value = true;
(void)slim_v_value;
{
SlimUnit slim_v_marked = {0};
slim_vec_set(slim_v_scheduled, slim_v_module, &slim_v_value); slim_v_marked = (SlimUnit){0};
(void)slim_v_marked;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_scheduler_schedule_layers(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_modules, SlimVec * slim_v_scheduled, int64_t slim_v_completed, int64_t slim_v_layer, int64_t slim_v_workers, SlimVec * slim_v_tasks, SlimVec * slim_v_batches, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_modules;
(void)slim_v_scheduled;
(void)slim_v_completed;
(void)slim_v_layer;
(void)slim_v_workers;
(void)slim_v_tasks;
(void)slim_v_batches;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_module_count = {0};
slim_v_module_count = ((*slim_v_modules)).len;
(void)slim_v_module_count;
{
bool slim_v_done = {0};
slim_v_done = slim_v_completed >= slim_v_module_count;
(void)slim_v_done;
if (slim_v_done) {
slim_result = true;
}
else {
{
SlimVec slim_v_ready = {0};
slim_v_ready = slim_vec_new(sizeof(int64_t), slim_allocation_region);
(void)slim_v_ready;
{
SlimUnit slim_v_collected = {0};
slim_v_collected = slim_fn_scheduler_collect_ready(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_scheduled, INT64_C(0), &slim_v_ready, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_collected;
{
int64_t slim_v_ready_count = {0};
slim_v_ready_count = (slim_v_ready).len;
(void)slim_v_ready_count;
{
bool slim_v_progress = {0};
slim_v_progress = slim_v_ready_count > INT64_C(0);
(void)slim_v_progress;
if (!slim_v_progress) {
slim_result = false;
}
else {
{
int64_t slim_v_start = {0};
slim_v_start = ((*slim_v_tasks)).len;
(void)slim_v_start;
{
SlimUnit slim_v_appended = {0};
slim_v_appended = slim_fn_scheduler_append_tasks(&slim_v_ready, INT64_C(0), slim_v_layer, slim_v_tasks, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_appended;
{
int64_t slim_v_end = {0};
slim_v_end = ((*slim_v_tasks)).len;
(void)slim_v_end;
{
SlimUnit slim_v_batched = {0};
slim_v_batched = slim_fn_scheduler_append_batches(slim_v_start, slim_v_end, slim_v_workers, slim_v_batches, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_batched;
{
SlimUnit slim_v_marked = {0};
slim_v_marked = slim_fn_scheduler_mark_ready(&slim_v_ready, INT64_C(0), slim_v_scheduled, slim_allocation_region);
(void)slim_v_marked;
{
int64_t slim_v_next_completed = {0};
slim_v_next_completed = slim_i64_add(slim_v_completed, slim_v_ready_count);
(void)slim_v_next_completed;
{
int64_t slim_v_next_layer = {0};
slim_v_next_layer = slim_i64_add(slim_v_layer, INT64_C(1));
(void)slim_v_next_layer;
slim_v_completed = slim_v_next_completed;
slim_v_layer = slim_v_next_layer;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_scheduler_bounded_workers(int64_t slim_v_requested, int64_t slim_v_module_count, SlimRegion *slim_region) {
(void)slim_v_requested;
(void)slim_v_module_count;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_positive = {0};
slim_v_positive = slim_v_requested > INT64_C(0);
(void)slim_v_positive;
{
int64_t slim_v_at_least_one = {0};
if (slim_v_positive) {
slim_v_at_least_one = slim_v_requested;
}
else {
slim_v_at_least_one = INT64_C(1);
}
(void)slim_v_at_least_one;
{
bool slim_v_above_ceiling = {0};
slim_v_above_ceiling = slim_v_at_least_one > INT64_C(4);
(void)slim_v_above_ceiling;
{
int64_t slim_v_implementation_bound = {0};
if (slim_v_above_ceiling) {
slim_v_implementation_bound = INT64_C(4);
}
else {
slim_v_implementation_bound = slim_v_at_least_one;
}
(void)slim_v_implementation_bound;
{
bool slim_v_above_modules = {0};
slim_v_above_modules = slim_v_implementation_bound > slim_v_module_count;
(void)slim_v_above_modules;
if (slim_v_above_modules) {
slim_result = slim_v_module_count;
}
else {
slim_result = slim_v_implementation_bound;
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static Slim_type_scheduler_Schedule slim_fn_scheduler_make_schedule(bool slim_v_valid, int64_t slim_v_workers, SlimVec slim_v_tasks, SlimVec slim_v_batches, SlimRegion *slim_region) {
(void)slim_v_valid;
(void)slim_v_workers;
(void)slim_v_tasks;
(void)slim_v_batches;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
Slim_type_scheduler_Schedule slim_result = {0};
{
bool slim_t_50799 = {0};
slim_t_50799 = slim_v_valid;
int64_t slim_t_50803 = {0};
slim_t_50803 = slim_v_workers;
SlimVec slim_t_50807 = {0};
slim_t_50807 = slim_v_tasks;
SlimVec slim_t_50811 = {0};
slim_t_50811 = slim_v_batches;
slim_result = (Slim_type_scheduler_Schedule){.slim_field_valid = slim_t_50799, .slim_field_workers = slim_t_50803, .slim_field_tasks = slim_t_50807, .slim_field_batches = slim_t_50811};
}
return slim_result;
}

static Slim_type_scheduler_Schedule slim_fn_scheduler_plan(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_requested, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_requested;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
Slim_type_scheduler_Schedule slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_modules = {0};
slim_v_modules = slim_vec_new(sizeof(int64_t), slim_allocation_region);
(void)slim_v_modules;
{
int64_t slim_v_entry_form = {0};
slim_v_entry_form = INT64_C(3);
(void)slim_v_entry_form;
{
int64_t slim_v_first_module = {0};
slim_v_first_module = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_entry_form, slim_allocation_region);
(void)slim_v_first_module;
{
SlimUnit slim_v_collected = {0};
slim_v_collected = slim_fn_scheduler_collect_modules(slim_v_tokens, slim_v_first_module, &slim_v_modules, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_collected;
{
int64_t slim_v_module_count = {0};
slim_v_module_count = (slim_v_modules).len;
(void)slim_v_module_count;
{
int64_t slim_v_workers = {0};
slim_v_workers = slim_fn_scheduler_bounded_workers(slim_v_requested, slim_v_module_count, slim_allocation_region);
(void)slim_v_workers;
{
SlimVec slim_v_scheduled = {0};
slim_v_scheduled = slim_vec_new(sizeof(bool), slim_allocation_region);
(void)slim_v_scheduled;
{
SlimUnit slim_v_initialized = {0};
slim_v_initialized = slim_fn_scheduler_fill_pending(slim_v_module_count, INT64_C(0), &slim_v_scheduled, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_initialized;
{
SlimVec slim_v_tasks = {0};
slim_v_tasks = slim_vec_new(sizeof(Slim_type_scheduler_Task), slim_allocation_region);
(void)slim_v_tasks;
{
SlimVec slim_v_batches = {0};
slim_v_batches = slim_vec_new(sizeof(Slim_type_scheduler_Batch), slim_allocation_region);
(void)slim_v_batches;
{
bool slim_v_valid = {0};
slim_v_valid = slim_fn_scheduler_schedule_layers(slim_v_source, slim_v_tokens, &slim_v_modules, &slim_v_scheduled, INT64_C(0), INT64_C(0), slim_v_workers, &slim_v_tasks, &slim_v_batches, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_valid;
slim_result = slim_fn_scheduler_make_schedule(slim_v_valid, slim_v_workers, slim_v_tasks, slim_v_batches, slim_allocation_region);
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_scheduler_parse_jobs_from(SlimBytes slim_v_input, int64_t slim_v_index, int64_t slim_v_length, int64_t slim_v_value, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_v_value;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_value;
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_input, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
bool slim_v_lower = {0};
slim_v_lower = slim_v_code >= INT64_C(48);
(void)slim_v_lower;
{
bool slim_v_upper = {0};
slim_v_upper = slim_v_code <= INT64_C(57);
(void)slim_v_upper;
{
bool slim_v_digit = {0};
slim_v_digit = slim_v_lower && slim_v_upper;
(void)slim_v_digit;
if (!slim_v_digit) {
slim_result = INT64_C(1);
}
else {
{
bool slim_v_already_capped = {0};
slim_v_already_capped = slim_v_value >= INT64_C(4);
(void)slim_v_already_capped;
{
int64_t slim_v_next_value = {0};
if (slim_v_already_capped) {
slim_v_next_value = INT64_C(4);
}
else {
{
int64_t slim_v_shifted = {0};
slim_v_shifted = slim_i64_mul(slim_v_value, INT64_C(10));
(void)slim_v_shifted;
{
int64_t slim_v_numeric = {0};
slim_v_numeric = slim_i64_sub(slim_v_code, INT64_C(48));
(void)slim_v_numeric;
{
int64_t slim_v_candidate = {0};
slim_v_candidate = slim_i64_add(slim_v_shifted, slim_v_numeric);
(void)slim_v_candidate;
{
bool slim_v_above = {0};
slim_v_above = slim_v_candidate > INT64_C(4);
(void)slim_v_above;
if (slim_v_above) {
slim_v_next_value = INT64_C(4);
}
else {
slim_v_next_value = slim_v_candidate;
}
}
}
}
}
}
(void)slim_v_next_value;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
slim_v_value = slim_v_next_value;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_scheduler_parse_jobs(SlimBytes slim_v_input, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
int64_t slim_v_length = {0};
slim_v_length = slim_bytes_len(slim_v_input);
(void)slim_v_length;
slim_result = slim_fn_scheduler_parse_jobs_from(slim_v_input, INT64_C(0), slim_v_length, INT64_C(0), slim_allocation_region);
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static Slim_type_query_State slim_fn_session_state_for_path(SlimBytes slim_v_path, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
Slim_type_query_State slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_manifest_bytes = {0};
slim_v_manifest_bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_manifest_bytes;
{
bool slim_v_manifest_read = {0};
slim_v_manifest_read = slim_read_file(slim_v_path, &slim_v_manifest_bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_manifest_read;
{
SlimBytes slim_v_manifest_source = {0};
slim_v_manifest_source = slim_bytes_freeze(slim_v_manifest_bytes);
(void)slim_v_manifest_source;
{
SlimVec slim_v_manifest_tokens = {0};
slim_v_manifest_tokens = slim_vec_new(sizeof(Slim_type_syntax_Token), slim_allocation_region);
(void)slim_v_manifest_tokens;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_lex(slim_v_manifest_source, INT64_C(0), &slim_v_manifest_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
{
int64_t slim_v_entry_form = {0};
slim_v_entry_form = INT64_C(3);
(void)slim_v_entry_form;
{
int64_t slim_v_modules = {0};
slim_v_modules = slim_fn_syntax_skip_form(&slim_v_manifest_tokens, slim_v_entry_form, slim_allocation_region);
(void)slim_v_modules;
{
SlimVec slim_v_module_tokens = {0};
slim_v_module_tokens = slim_vec_new(sizeof(Slim_type_syntax_Token), slim_allocation_region);
(void)slim_v_module_tokens;
{
SlimVec slim_v_loaded = {0};
slim_v_loaded = slim_vec_new(sizeof(Slim_type_project_LoadedModule), slim_allocation_region);
(void)slim_v_loaded;
{
int64_t slim_v_loaded_result = {0};
slim_v_loaded_result = slim_fn_project_load_project_modules(slim_v_path, slim_v_manifest_source, &slim_v_manifest_tokens, slim_v_modules, &slim_v_module_tokens, &slim_v_loaded, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_loaded_result;
slim_result = slim_fn_query_build_state(slim_v_manifest_source, &slim_v_manifest_tokens, &slim_v_module_tokens, &slim_v_loaded, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_session_print_work(Slim_type_query_Work slim_v_work, SlimRegion *slim_region) {
(void)slim_v_work;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
int64_t slim_v_parsed = {0};
slim_v_parsed = slim_v_work.slim_field_parsed;
(void)slim_v_parsed;
{
int64_t slim_v_lowered = {0};
slim_v_lowered = slim_v_work.slim_field_lowered;
(void)slim_v_lowered;
{
int64_t slim_v_checked = {0};
slim_v_checked = slim_v_work.slim_field_checked;
(void)slim_v_checked;
{
int64_t slim_v_generated = {0};
slim_v_generated = slim_v_work.slim_field_generated;
(void)slim_v_generated;
{
SlimUnit slim_v_first = {0};
slim_v_first = slim_print_i64(slim_v_parsed);
(void)slim_v_first;
{
SlimUnit slim_v_first_space = {0};
slim_v_first_space = slim_print_bytes(slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)));
(void)slim_v_first_space;
{
SlimUnit slim_v_second = {0};
slim_v_second = slim_print_i64(slim_v_lowered);
(void)slim_v_second;
{
SlimUnit slim_v_second_space = {0};
slim_v_second_space = slim_print_bytes(slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)));
(void)slim_v_second_space;
{
SlimUnit slim_v_third = {0};
slim_v_third = slim_print_i64(slim_v_checked);
(void)slim_v_third;
{
SlimUnit slim_v_third_space = {0};
slim_v_third_space = slim_print_bytes(slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)));
(void)slim_v_third_space;
{
SlimUnit slim_v_fourth = {0};
slim_v_fourth = slim_print_i64(slim_v_generated);
(void)slim_v_fourth;
{
SlimUnit slim_v_newline = {0};
slim_v_newline = slim_println(slim_bytes_static((const uint8_t *)"", (int64_t)(sizeof("") - 1)));
(void)slim_v_newline;
slim_result = INT64_C(0);
}
}
}
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_session_run(SlimBytes slim_v_initial_path, SlimBytes slim_v_updated_path, SlimRegion *slim_region) {
(void)slim_v_initial_path;
(void)slim_v_updated_path;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
Slim_type_query_State slim_v_previous_state = {0};
slim_v_previous_state = slim_fn_session_state_for_path(slim_v_initial_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_previous_state;
{
Slim_type_query_State slim_v_current_state = {0};
slim_v_current_state = slim_fn_session_state_for_path(slim_v_updated_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_current_state;
{
SlimVec slim_v_previous = {0};
slim_v_previous = slim_v_previous_state.slim_field_snapshots;
(void)slim_v_previous;
{
SlimVec slim_v_current = {0};
slim_v_current = slim_v_current_state.slim_field_snapshots;
(void)slim_v_current;
{
SlimVec slim_v_dependencies = {0};
slim_v_dependencies = slim_v_current_state.slim_field_dependencies;
(void)slim_v_dependencies;
{
Slim_type_query_Work slim_v_work = {0};
slim_v_work = slim_fn_query_measure_update(&slim_v_previous, &slim_v_current, &slim_v_dependencies, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_work;
slim_result = slim_fn_session_print_work(slim_v_work, slim_allocation_region);
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_session_run_recovery(SlimBytes slim_v_initial_path, SlimBytes slim_v_invalid_path, SlimBytes slim_v_recovered_path, SlimRegion *slim_region) {
(void)slim_v_initial_path;
(void)slim_v_invalid_path;
(void)slim_v_recovered_path;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_initial_errors = {0};
slim_v_initial_errors = slim_fn_project_check_project_path(slim_v_initial_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_initial_errors;
{
bool slim_v_initial_invalid = {0};
slim_v_initial_invalid = slim_v_initial_errors > INT64_C(0);
(void)slim_v_initial_invalid;
if (slim_v_initial_invalid) {
slim_result = slim_v_initial_errors;
}
else {
{
Slim_type_query_State slim_v_previous_state = {0};
slim_v_previous_state = slim_fn_session_state_for_path(slim_v_initial_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_previous_state;
{
int64_t slim_v_rejected = {0};
slim_v_rejected = slim_fn_project_check_project_path(slim_v_invalid_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_rejected;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_rejected > INT64_C(0);
(void)slim_v_invalid;
if (!slim_v_invalid) {
slim_result = INT64_C(65);
}
else {
{
int64_t slim_v_recovered_errors = {0};
slim_v_recovered_errors = slim_fn_project_check_project_path(slim_v_recovered_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_recovered_errors;
{
bool slim_v_recovered_invalid = {0};
slim_v_recovered_invalid = slim_v_recovered_errors > INT64_C(0);
(void)slim_v_recovered_invalid;
if (slim_v_recovered_invalid) {
slim_result = slim_v_recovered_errors;
}
else {
{
Slim_type_query_State slim_v_current_state = {0};
slim_v_current_state = slim_fn_session_state_for_path(slim_v_recovered_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_current_state;
{
SlimVec slim_v_previous = {0};
slim_v_previous = slim_v_previous_state.slim_field_snapshots;
(void)slim_v_previous;
{
SlimVec slim_v_current = {0};
slim_v_current = slim_v_current_state.slim_field_snapshots;
(void)slim_v_current;
{
SlimVec slim_v_dependencies = {0};
slim_v_dependencies = slim_v_current_state.slim_field_dependencies;
(void)slim_v_dependencies;
{
Slim_type_query_Work slim_v_work = {0};
slim_v_work = slim_fn_query_measure_update(&slim_v_previous, &slim_v_current, &slim_v_dependencies, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_work;
slim_result = slim_fn_session_print_work(slim_v_work, slim_allocation_region);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_syntax_token_kind(SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
Slim_type_syntax_Token slim_v_token = {0};
slim_v_token = ((Slim_type_syntax_Token *)((*slim_v_tokens)).data)[slim_vec_check_index(&((*slim_v_tokens)), slim_v_index)];
(void)slim_v_token;
slim_result = slim_v_token.slim_field_kind;
}
return slim_result;
}

static int64_t slim_fn_syntax_token_start(SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
Slim_type_syntax_Token slim_v_token = {0};
slim_v_token = ((Slim_type_syntax_Token *)((*slim_v_tokens)).data)[slim_vec_check_index(&((*slim_v_tokens)), slim_v_index)];
(void)slim_v_token;
slim_result = slim_v_token.slim_field_start;
}
return slim_result;
}

static int64_t slim_fn_syntax_token_end(SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
Slim_type_syntax_Token slim_v_token = {0};
slim_v_token = ((Slim_type_syntax_Token *)((*slim_v_tokens)).data)[slim_vec_check_index(&((*slim_v_tokens)), slim_v_index)];
(void)slim_v_token;
slim_result = slim_v_token.slim_field_end;
}
return slim_result;
}

static bool slim_fn_syntax_span_equal_chars(SlimBytes slim_v_source, int64_t slim_v_start, SlimBytes slim_v_text, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_start;
(void)slim_v_text;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index == slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = true;
}
else {
{
int64_t slim_v_source_index = {0};
slim_v_source_index = slim_i64_add(slim_v_start, slim_v_index);
(void)slim_v_source_index;
{
uint8_t slim_v_source_byte = {0};
slim_v_source_byte = slim_bytes_get(slim_v_source, slim_v_source_index);
(void)slim_v_source_byte;
{
uint8_t slim_v_text_byte = {0};
slim_v_text_byte = slim_bytes_get(slim_v_text, slim_v_index);
(void)slim_v_text_byte;
{
int64_t slim_v_source_code = {0};
slim_v_source_code = (int64_t)slim_v_source_byte;
(void)slim_v_source_code;
{
int64_t slim_v_text_code = {0};
slim_v_text_code = (int64_t)slim_v_text_byte;
(void)slim_v_text_code;
{
bool slim_v_same = {0};
slim_v_same = slim_v_source_code == slim_v_text_code;
(void)slim_v_same;
if (slim_v_same) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
else {
slim_result = false;
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_syntax_span_equal(SlimBytes slim_v_source, int64_t slim_v_start, int64_t slim_v_end, SlimBytes slim_v_text, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_start;
(void)slim_v_end;
(void)slim_v_text;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_span_length = {0};
slim_v_span_length = slim_i64_sub(slim_v_end, slim_v_start);
(void)slim_v_span_length;
{
int64_t slim_v_text_length = {0};
slim_v_text_length = slim_bytes_len(slim_v_text);
(void)slim_v_text_length;
{
bool slim_v_same_length = {0};
slim_v_same_length = slim_v_span_length == slim_v_text_length;
(void)slim_v_same_length;
if (slim_v_same_length) {
slim_result = slim_fn_syntax_span_equal_chars(slim_v_source, slim_v_start, slim_v_text, INT64_C(0), slim_v_span_length, slim_allocation_region);
}
else {
slim_result = false;
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_syntax_spans_equal_chars(SlimBytes slim_v_source, int64_t slim_v_left_start, int64_t slim_v_right_start, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_left_start;
(void)slim_v_right_start;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index == slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = true;
}
else {
{
int64_t slim_v_left_index = {0};
slim_v_left_index = slim_i64_add(slim_v_left_start, slim_v_index);
(void)slim_v_left_index;
{
int64_t slim_v_right_index = {0};
slim_v_right_index = slim_i64_add(slim_v_right_start, slim_v_index);
(void)slim_v_right_index;
{
uint8_t slim_v_left_byte = {0};
slim_v_left_byte = slim_bytes_get(slim_v_source, slim_v_left_index);
(void)slim_v_left_byte;
{
uint8_t slim_v_right_byte = {0};
slim_v_right_byte = slim_bytes_get(slim_v_source, slim_v_right_index);
(void)slim_v_right_byte;
{
int64_t slim_v_left_code = {0};
slim_v_left_code = (int64_t)slim_v_left_byte;
(void)slim_v_left_code;
{
int64_t slim_v_right_code = {0};
slim_v_right_code = (int64_t)slim_v_right_byte;
(void)slim_v_right_code;
{
bool slim_v_same = {0};
slim_v_same = slim_v_left_code == slim_v_right_code;
(void)slim_v_same;
if (slim_v_same) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
else {
slim_result = false;
}
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_syntax_spans_equal(SlimBytes slim_v_source, int64_t slim_v_left_start, int64_t slim_v_left_end, int64_t slim_v_right_start, int64_t slim_v_right_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_left_start;
(void)slim_v_left_end;
(void)slim_v_right_start;
(void)slim_v_right_end;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_left_length = {0};
slim_v_left_length = slim_i64_sub(slim_v_left_end, slim_v_left_start);
(void)slim_v_left_length;
{
int64_t slim_v_right_length = {0};
slim_v_right_length = slim_i64_sub(slim_v_right_end, slim_v_right_start);
(void)slim_v_right_length;
{
bool slim_v_same_length = {0};
slim_v_same_length = slim_v_left_length == slim_v_right_length;
(void)slim_v_same_length;
if (slim_v_same_length) {
slim_result = slim_fn_syntax_spans_equal_chars(slim_v_source, slim_v_left_start, slim_v_right_start, INT64_C(0), slim_v_left_length, slim_allocation_region);
}
else {
slim_result = false;
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_syntax_is_delimiter(int64_t slim_v_code, SlimRegion *slim_region) {
(void)slim_v_code;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
bool slim_v_whitespace = {0};
slim_v_whitespace = slim_v_code <= INT64_C(32);
(void)slim_v_whitespace;
{
bool slim_v_left = {0};
slim_v_left = slim_v_code == INT64_C(40);
(void)slim_v_left;
{
bool slim_v_right = {0};
slim_v_right = slim_v_code == INT64_C(41);
(void)slim_v_right;
{
bool slim_v_comment = {0};
slim_v_comment = slim_v_code == INT64_C(59);
(void)slim_v_comment;
{
bool slim_v_quote = {0};
slim_v_quote = slim_v_code == INT64_C(34);
(void)slim_v_quote;
{
bool slim_v_any_paren = {0};
slim_v_any_paren = slim_v_left || slim_v_right;
(void)slim_v_any_paren;
{
bool slim_v_any_control = {0};
slim_v_any_control = slim_v_whitespace || slim_v_comment;
(void)slim_v_any_control;
{
bool slim_v_any_delimiter = {0};
slim_v_any_delimiter = slim_v_any_paren || slim_v_any_control;
(void)slim_v_any_delimiter;
slim_result = slim_v_any_delimiter || slim_v_quote;
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_syntax_scan_atom(SlimBytes slim_v_input, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_index;
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_input, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
bool slim_v_delimiter = {0};
slim_v_delimiter = slim_fn_syntax_is_delimiter(slim_v_code, slim_allocation_region);
(void)slim_v_delimiter;
if (slim_v_delimiter) {
slim_result = slim_v_index;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_syntax_scan_string(SlimBytes slim_v_input, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_index;
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_input, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
bool slim_v_escape = {0};
slim_v_escape = slim_v_code == INT64_C(92);
(void)slim_v_escape;
if (slim_v_escape) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(2));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
else {
{
bool slim_v_quote = {0};
slim_v_quote = slim_v_code == INT64_C(34);
(void)slim_v_quote;
if (slim_v_quote) {
slim_result = slim_i64_add(slim_v_index, INT64_C(1));
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_syntax_scan_comment(SlimBytes slim_v_input, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_index;
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_input, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
bool slim_v_newline = {0};
slim_v_newline = slim_v_code == INT64_C(10);
(void)slim_v_newline;
if (slim_v_newline) {
slim_result = slim_i64_add(slim_v_index, INT64_C(1));
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static SlimUnit slim_fn_syntax_push_token(SlimVec * slim_v_tokens, int64_t slim_v_kind, int64_t slim_v_start, int64_t slim_v_end, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_kind;
(void)slim_v_start;
(void)slim_v_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
Slim_type_syntax_Token slim_v_token = {0};
{
int64_t slim_t_52907 = {0};
slim_t_52907 = slim_v_kind;
int64_t slim_t_52911 = {0};
slim_t_52911 = slim_v_start;
int64_t slim_t_52915 = {0};
slim_t_52915 = slim_v_end;
slim_v_token = (Slim_type_syntax_Token){.slim_field_kind = slim_t_52907, .slim_field_start = slim_t_52911, .slim_field_end = slim_t_52915};
}
(void)slim_v_token;
if (!slim_vec_push(slim_v_tokens, &(slim_v_token))) goto slim_allocation_failed; slim_result = (SlimUnit){0};
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_syntax_lex(SlimBytes slim_v_input, int64_t slim_v_index, SlimVec * slim_v_tokens, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_index;
(void)slim_v_tokens;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = slim_bytes_len(slim_v_input);
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_input, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
bool slim_v_whitespace = {0};
slim_v_whitespace = slim_v_code <= INT64_C(32);
(void)slim_v_whitespace;
if (slim_v_whitespace) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
else {
{
bool slim_v_comment = {0};
slim_v_comment = slim_v_code == INT64_C(59);
(void)slim_v_comment;
if (slim_v_comment) {
{
int64_t slim_v_content_start = {0};
slim_v_content_start = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_content_start;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_scan_comment(slim_v_input, slim_v_content_start, slim_v_length, slim_allocation_region);
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
else {
{
bool slim_v_left = {0};
slim_v_left = slim_v_code == INT64_C(40);
(void)slim_v_left;
if (slim_v_left) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
{
SlimUnit slim_v_pushed = {0};
slim_v_pushed = slim_fn_syntax_push_token(slim_v_tokens, INT64_C(0), slim_v_index, slim_v_next, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_pushed;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
else {
{
bool slim_v_right = {0};
slim_v_right = slim_v_code == INT64_C(41);
(void)slim_v_right;
if (slim_v_right) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
{
SlimUnit slim_v_pushed = {0};
slim_v_pushed = slim_fn_syntax_push_token(slim_v_tokens, INT64_C(1), slim_v_index, slim_v_next, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_pushed;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
else {
{
bool slim_v_quote = {0};
slim_v_quote = slim_v_code == INT64_C(34);
(void)slim_v_quote;
if (slim_v_quote) {
{
int64_t slim_v_content_start = {0};
slim_v_content_start = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_content_start;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_scan_string(slim_v_input, slim_v_content_start, slim_v_length, slim_allocation_region);
(void)slim_v_next;
{
SlimUnit slim_v_pushed = {0};
slim_v_pushed = slim_fn_syntax_push_token(slim_v_tokens, INT64_C(3), slim_v_index, slim_v_next, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_pushed;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_scan_atom(slim_v_input, slim_v_index, slim_v_length, slim_allocation_region);
(void)slim_v_next;
{
SlimUnit slim_v_pushed = {0};
slim_v_pushed = slim_fn_syntax_push_token(slim_v_tokens, INT64_C(2), slim_v_index, slim_v_next, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_pushed;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_syntax_token_equal(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimBytes slim_v_text, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_text;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_syntax_span_equal(slim_v_source, slim_v_start, slim_v_end, slim_v_text, slim_allocation_region);
}
}
return slim_result;
}

static int64_t slim_fn_syntax_skip_nested(SlimVec * slim_v_tokens, int64_t slim_v_index, int64_t slim_v_depth, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_depth;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_left = {0};
slim_v_left = slim_v_kind == INT64_C(0);
(void)slim_v_left;
{
bool slim_v_right = {0};
slim_v_right = slim_v_kind == INT64_C(1);
(void)slim_v_right;
if (slim_v_left) {
{
int64_t slim_v_next_depth = {0};
slim_v_next_depth = slim_i64_add(slim_v_depth, INT64_C(1));
(void)slim_v_next_depth;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
slim_v_depth = slim_v_next_depth;
goto slim_recur;
}
}
}
else {
if (slim_v_right) {
{
int64_t slim_v_next_depth = {0};
slim_v_next_depth = slim_i64_sub(slim_v_depth, INT64_C(1));
(void)slim_v_next_depth;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
{
bool slim_v_done = {0};
slim_v_done = slim_v_next_depth == INT64_C(0);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_next;
}
else {
slim_v_index = slim_v_next;
slim_v_depth = slim_v_next_depth;
goto slim_recur;
}
}
}
}
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_syntax_skip_form(SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_left = {0};
slim_v_left = slim_v_kind == INT64_C(0);
(void)slim_v_left;
if (slim_v_left) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_result = slim_fn_syntax_skip_nested(slim_v_tokens, slim_v_next, INT64_C(1), slim_allocation_region);
}
}
else {
slim_result = slim_i64_add(slim_v_index, INT64_C(1));
}
}
}
return slim_result;
}

static bool slim_fn_syntax_item_is(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_item, SlimBytes slim_v_text, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_item;
(void)slim_v_text;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_item, INT64_C(1));
(void)slim_v_head;
slim_result = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_v_text, slim_allocation_region);
}
return slim_result;
}

static bool slim_fn_syntax_binding_is_inout(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_cursor, int64_t slim_v_name_start, int64_t slim_v_name_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_cursor;
(void)slim_v_name_start;
(void)slim_v_name_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_is_inout = {0};
slim_v_is_inout = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"inout", (int64_t)(sizeof("inout") - 1)), slim_allocation_region);
(void)slim_v_is_inout;
if (slim_v_is_inout) {
{
int64_t slim_v_name_index = {0};
slim_v_name_index = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name_index;
{
int64_t slim_v_candidate_start = {0};
slim_v_candidate_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_name_index, slim_allocation_region);
(void)slim_v_candidate_start;
{
int64_t slim_v_candidate_end = {0};
slim_v_candidate_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_name_index, slim_allocation_region);
(void)slim_v_candidate_end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_spans_equal(slim_v_source, slim_v_candidate_start, slim_v_candidate_end, slim_v_name_start, slim_v_name_end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_syntax_name_is_inout(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_name, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_name;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_cursor = {0};
slim_v_cursor = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_cursor;
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_start;
{
int64_t slim_v_name_end = {0};
slim_v_name_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_end;
slim_result = slim_fn_syntax_binding_is_inout(slim_v_source, slim_v_tokens, slim_v_params, slim_v_cursor, slim_v_name_start, slim_v_name_end, slim_allocation_region);
}
}
}
return slim_result;
}

static bool slim_fn_syntax_first_byte_is_number(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_start;
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_source, slim_v_start);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
bool slim_v_minus = {0};
slim_v_minus = slim_v_code == INT64_C(45);
(void)slim_v_minus;
{
bool slim_v_lower = {0};
slim_v_lower = slim_v_code >= INT64_C(48);
(void)slim_v_lower;
{
bool slim_v_upper = {0};
slim_v_upper = slim_v_code <= INT64_C(57);
(void)slim_v_upper;
{
bool slim_v_digit = {0};
slim_v_digit = slim_v_lower && slim_v_upper;
(void)slim_v_digit;
slim_result = slim_v_minus || slim_v_digit;
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_syntax_find_function_params(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_callee_start, int64_t slim_v_callee_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_callee_start;
(void)slim_v_callee_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_name_start = {0};
slim_v_name_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_start;
{
int64_t slim_v_name_end = {0};
slim_v_name_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_spans_equal(slim_v_source, slim_v_name_start, slim_v_name_end, slim_v_callee_start, slim_v_callee_end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_i64_add(slim_v_cursor, INT64_C(3));
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_syntax_span_count_byte(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, int64_t slim_v_code, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_index;
(void)slim_v_end;
(void)slim_v_code;
(void)slim_v_count;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_source, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_current = {0};
slim_v_current = (int64_t)slim_v_byte;
(void)slim_v_current;
{
bool slim_v_same = {0};
slim_v_same = slim_v_current == slim_v_code;
(void)slim_v_same;
{
int64_t slim_v_next_count = {0};
if (slim_v_same) {
slim_v_next_count = slim_i64_add(slim_v_count, INT64_C(1));
}
else {
slim_v_next_count = slim_v_count;
}
(void)slim_v_next_count;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
slim_v_count = slim_v_next_count;
goto slim_recur;
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_syntax_scalar_type_kind(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_type_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_type_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_unit_type = {0};
slim_v_unit_type = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_type_index, slim_bytes_static((const uint8_t *)"Unit", (int64_t)(sizeof("Unit") - 1)), slim_allocation_region);
(void)slim_v_unit_type;
if (slim_v_unit_type) {
slim_result = INT64_C(0);
}
else {
{
bool slim_v_bool_type = {0};
slim_v_bool_type = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_type_index, slim_bytes_static((const uint8_t *)"Bool", (int64_t)(sizeof("Bool") - 1)), slim_allocation_region);
(void)slim_v_bool_type;
if (slim_v_bool_type) {
slim_result = INT64_C(1);
}
else {
{
bool slim_v_u8_type = {0};
slim_v_u8_type = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_type_index, slim_bytes_static((const uint8_t *)"U8", (int64_t)(sizeof("U8") - 1)), slim_allocation_region);
(void)slim_v_u8_type;
if (slim_v_u8_type) {
slim_result = INT64_C(2);
}
else {
{
bool slim_v_i64_type = {0};
slim_v_i64_type = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_type_index, slim_bytes_static((const uint8_t *)"I64", (int64_t)(sizeof("I64") - 1)), slim_allocation_region);
(void)slim_v_i64_type;
if (slim_v_i64_type) {
slim_result = INT64_C(3);
}
else {
{
bool slim_v_bytes_type = {0};
slim_v_bytes_type = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_type_index, slim_bytes_static((const uint8_t *)"Bytes", (int64_t)(sizeof("Bytes") - 1)), slim_allocation_region);
(void)slim_v_bytes_type;
if (slim_v_bytes_type) {
slim_result = INT64_C(4);
}
else {
slim_result = INT64_C(-1);
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static Slim_type_ir_DeclarationKind slim_fn_syntax_declaration_kind(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_form, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_form;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
Slim_type_ir_DeclarationKind slim_result = {0};
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_form, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_function = {0};
slim_v_function = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function;
if (slim_v_function) {
{
Slim_type_ir_DeclarationKind slim_v_kind = {0};
{
slim_v_kind = (Slim_type_ir_DeclarationKind){.tag = SLIM_TAG_ir_DeclarationKind_Function};
}
(void)slim_v_kind;
slim_result = slim_v_kind;
}
}
else {
{
bool slim_v_record = {0};
slim_v_record = slim_fn_syntax_token_equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"record", (int64_t)(sizeof("record") - 1)), slim_allocation_region);
(void)slim_v_record;
if (slim_v_record) {
{
Slim_type_ir_DeclarationKind slim_v_kind = {0};
{
slim_v_kind = (Slim_type_ir_DeclarationKind){.tag = SLIM_TAG_ir_DeclarationKind_Record};
}
(void)slim_v_kind;
slim_result = slim_v_kind;
}
}
else {
{
Slim_type_ir_DeclarationKind slim_v_kind = {0};
{
slim_v_kind = (Slim_type_ir_DeclarationKind){.tag = SLIM_TAG_ir_DeclarationKind_Variant};
}
(void)slim_v_kind;
slim_result = slim_v_kind;
}
}
}
}
}
}
return slim_result;
}

static SlimUnit slim_fn_syntax_index_declarations_from(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_declarations, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_declarations;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_token = {0};
slim_v_token = slim_fn_syntax_token_kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_token;
{
bool slim_v_done = {0};
slim_v_done = slim_v_token == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
bool slim_v_form = {0};
slim_v_form = slim_v_token == INT64_C(0);
(void)slim_v_form;
if (!slim_v_form) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_skip_form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
Slim_type_ir_DeclarationKind slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_declaration_kind(slim_v_source, slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
Slim_type_ir_Declaration slim_v_declaration = {0};
{
Slim_type_ir_DeclarationKind slim_t_54724 = {0};
slim_t_54724 = slim_v_kind;
int64_t slim_t_54728 = {0};
slim_t_54728 = slim_v_cursor;
int64_t slim_t_54732 = {0};
slim_t_54732 = slim_v_name;
int64_t slim_t_54736 = {0};
slim_t_54736 = slim_v_next;
slim_v_declaration = (Slim_type_ir_Declaration){.slim_field_kind = slim_t_54724, .slim_field_form = slim_t_54728, .slim_field_name = slim_t_54732, .slim_field_next = slim_t_54736};
}
(void)slim_v_declaration;
{
SlimUnit slim_v_pushed = {0};
if (!slim_vec_push(slim_v_declarations, &(slim_v_declaration))) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
(void)slim_v_pushed;
slim_v_cursor = slim_v_next;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimVec slim_fn_syntax_index_declarations(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimVec slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_declarations = {0};
slim_v_declarations = slim_vec_new(sizeof(Slim_type_ir_Declaration), slim_allocation_region);
(void)slim_v_declarations;
{
SlimUnit slim_v_indexed = {0};
slim_v_indexed = slim_fn_syntax_index_declarations_from(slim_v_source, slim_v_tokens, INT64_C(3), &slim_v_declarations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_indexed;
slim_result = slim_v_declarations;
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_text_append_span(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_index;
(void)slim_v_end;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_source, slim_v_index);
(void)slim_v_byte;
{
SlimUnit slim_v_pushed = {0};
if (!slim_vec_push(slim_v_output, &(slim_v_byte))) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
(void)slim_v_pushed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_text_append_text_chars(SlimBytes slim_v_text, int64_t slim_v_index, int64_t slim_v_length, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_text;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_text, slim_v_index);
(void)slim_v_byte;
{
SlimUnit slim_v_pushed = {0};
if (!slim_vec_push(slim_v_output, &(slim_v_byte))) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
(void)slim_v_pushed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_v_index = slim_v_next;
goto slim_recur;
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_text_append_text(SlimVec * slim_v_output, SlimBytes slim_v_text, SlimRegion *slim_region) {
(void)slim_v_output;
(void)slim_v_text;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_length = {0};
slim_v_length = slim_bytes_len(slim_v_text);
(void)slim_v_length;
slim_result = slim_fn_text_append_text_chars(slim_v_text, INT64_C(0), slim_v_length, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_text_append_nonnegative_i64(int64_t slim_v_value, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_value;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_multiple = {0};
slim_v_multiple = slim_v_value >= INT64_C(10);
(void)slim_v_multiple;
if (slim_v_multiple) {
{
int64_t slim_v_prefix = {0};
slim_v_prefix = slim_i64_div(slim_v_value, INT64_C(10));
(void)slim_v_prefix;
{
int64_t slim_v_remainder = {0};
slim_v_remainder = slim_i64_rem(slim_v_value, INT64_C(10));
(void)slim_v_remainder;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_text_append_nonnegative_i64(slim_v_prefix, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
{
int64_t slim_v_code = {0};
slim_v_code = slim_i64_add(slim_v_remainder, INT64_C(48));
(void)slim_v_code;
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_i64_to_u8(slim_v_code);
(void)slim_v_byte;
if (!slim_vec_push(slim_v_output, &(slim_v_byte))) goto slim_allocation_failed; slim_result = (SlimUnit){0};
}
}
}
}
}
}
else {
{
int64_t slim_v_code = {0};
slim_v_code = slim_i64_add(slim_v_value, INT64_C(48));
(void)slim_v_code;
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_i64_to_u8(slim_v_code);
(void)slim_v_byte;
if (!slim_vec_push(slim_v_output, &(slim_v_byte))) goto slim_allocation_failed; slim_result = (SlimUnit){0};
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_text_append_token(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_token_start(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_token_end(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_text_append_span(slim_v_source, slim_v_start, slim_v_end, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
slim_allocation_failed: ;
return slim_result;
}

int main(int argc, char **argv) {
SlimAllocStatus slim_status;
slim_alloc_status_init(&slim_status);
SlimRegion slim_root;
slim_rt_init(&slim_root, &slim_status);
SlimVec slim_args = slim_vec_new(sizeof(SlimBytes), &slim_root);
for (int slim_i = 0; slim_i < argc; ++slim_i) {
SlimBytes slim_arg = slim_bytes_static((const uint8_t *)argv[slim_i], (int64_t)strlen(argv[slim_i]));
if (!slim_vec_push(&slim_args, &slim_arg)) {
slim_alloc_report(&slim_status);
slim_rt_shutdown();
return 71;
}
}
int64_t slim_exit_code = slim_fn_main(slim_args, &slim_root);
if (slim_region_failed(&slim_root)) {
slim_alloc_report(&slim_status);
slim_rt_shutdown();
return 71;
}
if (slim_exit_code < 0 || slim_exit_code > 255) {
slim_rt_trap("main result is outside 0..255");
}
slim_rt_shutdown();
return (int)slim_exit_code;
}

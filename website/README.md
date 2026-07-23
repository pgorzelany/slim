# SLIM website

Minimal four-route documentation site for SLIM — Small Language for Intelligent
Machines.

The site does not own language facts. `npm run generate` reads the canonical
repository documents, accepted surface ledger, compiler version, and tested
examples, then creates the build inputs for:

- `/`
- `/learn`
- `/reference`
- `/status`
- `/reference/surface.json`
- `/llms.txt`

## Commands

```sh
npm install
npm run dev
npm test
npm run lint
npm run build
```

Generated content and deployment output are ignored. The compiler and runtime
do not depend on this project.

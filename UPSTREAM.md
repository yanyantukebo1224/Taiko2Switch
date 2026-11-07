# YataiDON upstream integration

This project integrates [Yonokid/YataiDON](https://github.com/Yonokid/YataiDON).

- Upstream repository: https://github.com/Yonokid/YataiDON
- Upstream branch at migration start: master
- Pinned upstream commit: 4c87f294a1a791aacb6dc14a2dc31f3675b92f8f
- License: GNU GPL-3.0-or-later only if explicitly stated by upstream; otherwise GNU GPL-3.0 as supplied in the upstream LICENSE file.

The source is tracked as a Git submodule at `vendor/YataiDON`. To fetch it locally:

```sh
git submodule update --init --recursive
```

Keep the submodule pinned to a reviewed upstream commit. Record local patches separately and preserve upstream copyright notices and license files.

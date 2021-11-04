# Third party libraries

The following lists C/C++ libraries which are bundled and used by Goost.

## ExoQuant
- Upstream: https://github.com/exoticorn/exoquant
- Version: v0.7 (4ec54ab) (Feb 20, 2018)
- License: MIT

### Changes:
- removed `#include "malloc.h" as it's not available in iOS and MacOS.

## HQX
- Upstream: https://github.com/brunexgeek/hqx
- Version: git (0c1caa3120685c4607cc1a312e7877d8dc3828ab, 2020), file structure differs
- License: Apache 2.0

### Notes
- Godot 3.x includes `hq2x` version only. This module includes `hq3x` as well.

## Leptonica
- Upstream: https://github.com/DanBloomberg/leptonica
- Version: git (2bb50cb34b8dc6243c4e82d227bc903d83b59f04, 2021), file structure differs (extracted `src`)
- License: [BSD-2-Clause](leptonica/leptonica-license.txt)

### Notes
- Function prototypes are generated inline in `allheaders.h` with the suggested
  `xtractprotos` utility to allow building only the necessary sources while also
  reducing binary size.

## giflib
- Upstream: http://sourceforge.net/projects/giflib
- Version: 5.2.1
- License: MIT
- See [modules/gif/thirdparty/README.md](../modules/gif/thirdparty/README.md)
  for more details.

## Clipper
- Upstream: https://sourceforge.net/projects/polyclipping/
- Version: 10.0.0 r539 (sandbox)
- License: Boost Software License - Version 1.0

### Changes
- `Clipper.GetBounds` bug temporarily fixed:
    - https://sourceforge.net/p/polyclipping/bugs/176/
- Workaround for ill-formed `Clipper.PolyTree`, simply replaced with `Clipper.PolyPath`: 
    - https://sourceforge.net/p/polyclipping/bugs/177/
- Disabled exceptions (similarly to Godot's `clipper-no-exceptions.patch`)
- Removed Windows-specific debugging calls.
- Simple conditional version defines based on whether the older version was used.

See [`clipper.10.0.0-r539-no-except.patch`](clipper/clipper.10.0.0-r539-no-except.patch)
file for the exact changes.

## libgit2
- Upstream: https://github.com/libgit2/libgit2
- Version: v1.1.1
- License: GPLv2-only with linking exception

# Third-party components

The libraries below are vendored under `external/` and are covered by their own
licences, not by the COPYRIGHT notice at the root of this repository.

| Component | Version | Upstream | Licence |
| --- | --- | --- | --- |
| GLM | 0.9.9.8 | https://github.com/g-truc/glm | MIT, or The Happy Bunny License, at your option. Full text in `glm/copying.txt` |
| glad | 0.1.34, generated 2021-06-25, gl 4.4 core | https://github.com/Dav1dde/glad | Loader generator is MIT (David Herberth, 2013-2020). The Khronos specifications it is generated from are Apache-2.0, and `KHR/khrplatform.h` is MIT (The Khronos Group), notice inline in that file |
| stb_image | v2.27 | https://github.com/nothings/stb | Dual: MIT (Sean Barrett, 2017) or public domain (Unlicense), at your option. Full text at the end of `stb/stb_image.h` |
| AudioFile | 2017 | https://github.com/adamstark/AudioFile | MIT (Adam Stark, 2017), full text inline in `AudioFile/AudioFile.h` |
| OpenAL headers | openal-soft, release not pinned | https://github.com/kcat/openal-soft | GNU Library General Public License v2, full text in `AL/COPYING`. These headers are a build fallback only; `CMakeLists.txt` prefers a system OpenAL when one is present |

Media attribution is recorded separately in the COPYRIGHT file at the root and
in `media/sound_effects/info.txt`.

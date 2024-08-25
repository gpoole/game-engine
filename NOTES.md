# Build it

```sh
bin/setup-and-build.sh
```

## Dependencies

Dependencies managed with Conan:

```sh
conan install . --output-folder=build --build=missing
```

### imgui

imgui is not managed with Conan, I could not figure out how to get their sample
working and the only working demo I found involved some weird copy step that
conan didn't like. I have just included the code and wired it up in CMake
manually.

# References

https://github.com/Gaetz/SDL-OpenGL/
https://github.com/SerenityOS/serenity/blob/master/Documentation/CodingStyle.md
http://tfc.duke.free.fr/coding/md2-specs-en.html
https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-introduction

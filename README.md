**The porting to C++ try for the Darknet neural network repository**

Original repository taken from [https://github.com/AlexeyAB/darknet](https://github.com/AlexeyAB/darknet)

Some parts of the repository taken from the C++ template project located at [https://github.com/yuriysydor1991/cpp-app-template.git](https://github.com/yuriysydor1991/cpp-app-template.git)

**Work in progress. Do not expect project to work in the current state**

See more at the [kytok.org.ua](http://www.kytok.org.ua/)

# Recommended images annotator

In order to train the network on your own data set you'll need annotated images with the bounding box labels in the Darknet/YOLO format. It's recommended to prepare such annotations with the `ImagesAnnotator` tool available at [https://github.com/yuriysydor1991/ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator).

# Documentation contents

**Document is under the refinement**

The very same sections are also available in Ukrainian under [doc/README.uk_UA.md](/doc/README.uk_UA.md). The whole documentation may also be generated as a browsable HTML site (including the rendered PlantUML class diagrams) with Doxygen - see [Documentation build](/doc/sections/en_US/5-4-documentation-build.md).

1. [Cloning the C++ template project](/doc/sections/en_US/1-cloning-the-cxx-template-project.md)
1. [Forking and replacing the origin](/doc/sections/en_US/2-forking-and-replacing-the-origin.md)
1. [Requirements](/doc/sections/en_US/3-requirements.md)
    1. [Required tools for the GNU/Linux based OS](/doc/sections/en_US/3-1-required-tools-for-the-GNU-Linux-based-OS.md)
    1. [Required tools for the MS Windows based OS](/doc/sections/en_US/3-2-required-tools-for-the-MS-Windows-based-OS.md)
    1. [Optional for the tests](/doc/sections/en_US/3-3-optional-for-the-tests.md)
    1. [Optional for the documentation](/doc/sections/en_US/3-4-optional-for-the-documentation.md)
    1. [Optional for the code formatting](/doc/sections/en_US/3-5-optional-for-the-code-formatting.md)
    1. [Optional for the code analyzer (cppcheck)](/doc/sections/en_US/3-6-optional-for-the-code-analyzer-cppcheck.md)
    1. [Optional for the code analyzer with clang-tidy](/doc/sections/en_US/3-7-optional-for-the-code-analyzer-with-clang-tidy.md)
    1. [Optional for the memory check with Valgrind](/doc/sections/en_US/3-8-optional-for-the-memory-checkwith-Valgrind.md)
    1. [Optional for the flatpak packager](/doc/sections/en_US/3-9-optional-for-the-flatpak-packager.md)
    1. [Optional for the Docker container runs](/doc/sections/en_US/3-10-optional-for-docker-container-runs.md)
    1. [Optional for the snap packager](/doc/sections/en_US/3-11-optional-for-snap-packager.md)
1. [Project structure](/doc/sections/en_US/4-project-structure.md)
    1. [Implement code straight away!](/doc/sections/en_US/4-1-implement-code-straight-away.md)
    1. [Changing the project and executable name](/doc/sections/en_US/4-2-changing-the-project-and-executable-name.md)
    1. [Version tracking and other project parameters](/doc/sections/en_US/4-3-version-tracking-and-other-project-parameters.md)
    1. [Minimal possible versions](/doc/sections/en_US/4-6-minimal-possible-versions.md)
    1. [Project tests](/doc/sections/en_US/4-4-project-tests.md)
        1. [Google Test](/doc/sections/en_US/4-4-1-google-test.md)
    1. [Extensions](/doc/sections/en_US/4-5-extensions.md)
    1. [Project components and architecture](/doc/sections/en_US/4-7-components-and-architecture.md)
        1. [Application runtime components](/doc/sections/en_US/4-7-1-application-runtime-components.md)
        1. [Darknet adaptor components](/doc/sections/en_US/4-7-2-darknet-adaptor-components.md)
        1. [Supporting components](/doc/sections/en_US/4-7-3-supporting-components.md)
1. [Project build](/doc/sections/en_US/5-project-build.md)
    1. [IDE build](/doc/sections/en_US/5-1-IDE-build.md)
    1. [Command line build](/doc/sections/en_US/5-2-command-line-build.md)
    1. Enabling testing
        1. [Enabling unit testing](/doc/sections/en_US/5-3-1-enabling-unit-testing.md)
        1. [Disabling system GTest probe](/doc/sections/en_US/5-3-2-disabling-system-GTest-probe.md)
    1. [Documentation build](/doc/sections/en_US/5-4-documentation-build.md)
    1. [Configuring the documentation install support](/doc/sections/en_US/5-5-configuring-the-documentation-install-support.md)
    1. [Enabling and performing code formatting target](/doc/sections/en_US/5-6-enabling-and-performing-code-formatting-target.md)
    1. [Enabling the static code analyzer target with cppcheck](/doc/sections/en_US/5-7-enabling-the-static-code-analyzer-target-with-cppcheck.md)
    1. [Enabling the static code analyzer with clang-tidy](/doc/sections/en_US/5-8-enabling-static-code-analyzer-with-clang-tidy.md)
    1. [Enabling the dynamic memory check target with valgrind](/doc/sections/en_US/5-9-enabling-the-dynamic-memory-check-target-with-valgrind.md)
    1. [Enabling DEB package generation with cpack](/doc/sections/en_US/5-10-enabling-DEB-package-generation-with-cpack.md)
    1. [Enabling the flatpak package generation support](/doc/sections/en_US/5-11-enabling-the-flatpak-package-generation-support.md)
    1. [Enabling the Docker container build and run](/doc/sections/en_US/5-12-enabling-the-docker-container-build-and-run.md)
    1. [Enabling sanitizers](/doc/sections/en_US/5-13-enabling-sanitizers.md)
    1. [Enabling the libcurl](/doc/sections/en_US/5-14-enabling-libcurl.md)
    1. [Enabling gprof profiler analysis](/doc/sections/en_US/5-15-enabling-gprof-profiler-analysis.md)
    1. [Enabling vagrind's callgrind profiler analysis](/doc/sections/en_US/5-16-enabling-valgrinds-callgrind-profiler-analysis.md)
    1. [Enabling Jenkins pipeline inside Docker container](/doc/sections/en_US/5-17-enabling-Jenkins-pipeline-inside-Docker-container.md)
    1. [Enabling the nlohmann json library](/doc/sections/en_US/5-18-enabling-the-nlohmann-json-library.md)
    1. [Enabling the snap packager](/doc/sections/en_US/5-19-enabling-the-snap-package-manager.md)
    1. [Enabling FreeBSD pkg package generation with cpack](/doc/sections/en_US/5-20-enabling-FreeBSD-pkg-package-generation-with-cpack.md)
    1. [Enabling WIX MSI package generation with cpack](/doc/sections/en_US/5-21-enabling-WIX-MSI-package-generation-with-cpack.md)
    1. [Enabling RPM package generation with cpack](/doc/sections/en_US/5-22-enabling-RPM-package-generation-with-cpack.md)
    1. [Enabling the zlib library](/doc/sections/en_US/5-23-enabling-the-zlib-library.md)
    1. [Enabling the libpng library](/doc/sections/en_US/5-24-enabling-the-libpng-library.md)
    1. [Enabling the libjpeg library](/doc/sections/en_US/5-25-enabling-the-libjpeg-library.md)
    1. [Enabling the libwebp library](/doc/sections/en_US/5-26-enabling-the-libwebp-library.md)
    1. [Enabling the lunasvg library (SVG)](/doc/sections/en_US/5-27-enabling-the-lunasvg-library.md)
    1. [Enabling the giflib library (GIF)](/doc/sections/en_US/5-28-enabling-the-giflib-library.md)
    1. [Enabling the libtiff library (TIFF)](/doc/sections/en_US/5-29-enabling-the-libtiff-library.md)
    1. [Enabling the OpenEXR library (EXR / HDR)](/doc/sections/en_US/5-30-enabling-the-openexr-library.md)
    1. [Enabling the OpenJPEG library (JPEG 2000)](/doc/sections/en_US/5-31-enabling-the-openjpeg-library.md)
    1. [Enabling the libavif library (AVIF)](/doc/sections/en_US/5-32-enabling-the-libavif-library.md)
    1. [Enabling the libheif library (HEIF/HEIC)](/doc/sections/en_US/5-33-enabling-the-libheif-library.md)
1. [Run the executable](/doc/sections/en_US/6-run-the-executable.md)
    1. [IDE run](/doc/sections/en_US/6-1-IDE-run.md)
    1. Command line run
        1. [Searching for the generated executable](/doc/sections/en_US/6-2-1-searching-for-the-generated-executable.md)
        1. [Starting the generated executable](/doc/sections/en_US/6-2-2-starting-the-generated-executable.md)
        1. [Available command line parameters](/doc/sections/en_US/6-2-3-available-command-line-parameters.md)
    1. Tests run
        1. [Run tests by the ctest](/doc/sections/en_US/6-3-1-run-tests-by-the-ctest.md)
1. [Installing](/doc/sections/en_US/7-installing.md)

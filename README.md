**The porting to C++ try for the Darknet neural network repository**

Original repository taken from [https://github.com/AlexeyAB/darknet](https://github.com/AlexeyAB/darknet)

Some parts of the repository taken from the C++ template project located at [https://github.com/yuriysydor1991/cpp-app-template.git](https://github.com/yuriysydor1991/cpp-app-template.git)

**Work in progress. Do not expect project to work in the current state**

# Recommended images annotator

In order to train the network on your own data set you'll need annotated images with the bounding box labels in the Darknet/YOLO format. It's recommended to prepare such annotations with the `ImagesAnnotator` tool available at [https://github.com/yuriysydor1991/ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator).

# Documentation

This file is the project home page. The detailed documentation is split into the
sections below. The very same sections are also available in Ukrainian under
[doc/README.uk_UA.md](/doc/README.uk_UA.md).

1. [Requirements](/doc/sections/en_US/1-requirements.md)
1. [Project structure](/doc/sections/en_US/2-project-structure.md)
1. [Project components and architecture](/doc/sections/en_US/3-components-and-architecture.md)
    1. [Application runtime components](/doc/sections/en_US/3-1-application-runtime-components.md)
    1. [Darknet adaptor components](/doc/sections/en_US/3-2-darknet-adaptor-components.md)
    1. [Supporting components](/doc/sections/en_US/3-3-supporting-components.md)
1. [Project build](/doc/sections/en_US/4-project-build.md)
1. [Run the executable](/doc/sections/en_US/5-run-the-executable.md)
1. [Installing](/doc/sections/en_US/6-installing.md)

The same documentation may be generated as a browsable HTML site (including the
rendered PlantUML class diagrams) with Doxygen - see the
[Documentation build](/doc/sections/en_US/4-project-build.md#documentation-build)
section.

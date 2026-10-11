# ImagesAnnotator project files as a training source

**Status:** superseded on 2026-10-10 by the dxxwjz2p project file (see the [The dxxwjz2p project file format](/doc/sections/en_US/4-7-4-network-weights-files.md#the-dxxwjz2p-project-file-format) section). The project file keeps the network and the annotations of the ImagesAnnotator project structure together, and the training reads them through a native dataset seam like the one of route 2 below (the `get_paths()` and the `read_boxes()` hooks of the `TrainingDataRegistry`), with no DataDrivers dependency. The ImagesAnnotator project file itself isn't read: its annotations are the dxxwjz2p `annotations` member. The analysis below is kept for the record.

**Analyzed on 2026-10-05**, against:

- darknetxx: the working tree on top of `4d5cbbd4`, with the `src/lib` restructuring still uncommitted;
- [ImagesAnnotator-DataDrivers](https://github.com/yuriysydor1991/ImagesAnnotator-DataDrivers) `67b7e72` (0.12.0);
- [ImagesAnnotator-DataExporters](https://github.com/yuriysydor1991/ImagesAnnotator-DataExporters) `c87fe4d` (0.12.0);
- [ImagesAnnotator](https://github.com/yuriysydor1991/ImagesAnnotator) `7771cf9`.

The line numbers below are from those revisions. Once they drift, search by the function names.

## Goal

Train straight from an ImagesAnnotator annotations project file, the JSON document the DataDrivers library reads and writes, without exporting a YOLO4 folder first. The YOLO4 folder stays supported alongside it.

## Verdict

Feasible. The project file holds everything a training needs except the network cfg. The DataExporters YOLO4 exporter already fixes the conversion rules, and DataDrivers does the parsing. The real work is in the darknet C core, which finds the labels of an image in exactly one way: the `.txt` file that sits next to the image path.

## Today's path

```
ImagesAnnotator -- DataExporters Yolo42FolderExporter --> YOLO4 folder
YOLO4 folder -- Yolo4Folder::enter() --> LibraryContext -- BackgroundTrainer --> LibMain
  --> DarknetController --> ADetectorTrainer --> train_detector_ctx()
      prepare_train_ctx():     get_paths(train list)
      load_data_detection():   fill_truth_detection() -> replace_image_to_label() + read_boxes()
      validate_detector_map(): get_paths(valid list), replace_image_to_label() + read_boxes()
```

`Yolo4Folder::enter()` makes the folder the working directory and fills `train_cfg`, `names`, `latest_weights` and `chart_path`. The lists come from the cfg `[net]` defaults (`train=data/train.txt`; `valid=` falls back to the train list), resolved relative to that directory. Nothing sets `datacfg`, so `obj.data` is never read.

What the exporter writes (`src/exporters/Yolo4/Yolo42FolderExporter.cpp` of DataExporters):

| Path | Content |
|---|---|
| `cfg/yolov4-obj.cfg` | written by `Yolov4CfgWriter`: `classes`, `filters=(classes+5)*3`, `max_batches=max(minIterations, classes * iterationsPerClass, images)` |
| `data/obj.names` | `get_available_annotations()`, one name per line |
| `data/<image>`, `data/<stem>.txt` | the copied images and their labels |
| `data/train.txt`, `data/val.txt` | the same `data/<image>` lines in both |
| `data/obj.data` | written, but darknetxx doesn't read it |
| `backup/` | the weights |

## The project file in short

One JSON document. The root holds `"ImagesAnnotator": {"version"}` and `annotations[]`, an array of image folders: `{ "db": {"path"}, "annotations": [...] }`. `db.path` is an absolute directory or a web page URL. Each folder holds image records: `{ "rel_path", "width", "height", "scale_factor", "annotations": [...] }`. The image `width`/`height` are optional and default to 0. Each record holds named rectangles: `{ "name", "rect": {"x", "y", "width", "height"} }`, in image pixels, where `(x, y)` is the top-left corner. The full specification is in DataDrivers `doc/sections/en_US/4-project-structure/4-10-the-annotations-project-json-format.md`.

## Mapping

| The training needs | YOLO4 folder | Project file (through DataDrivers) |
|---|---|---|
| network cfg | `cfg/*.cfg` | nothing: `--train-cfg` (a `.cfg` or a `.dxxwjz2`, which embeds the cfg) |
| class names, ordered | `data/obj.names` | `get_available_annotations()` |
| train list | `data/train.txt` | the records' `get_full_path()` |
| valid list | `data/val.txt` | none (the exporter writes val = train) |
| boxes | `<image>.txt` | the records' `rects` |
| backup, chart | in the folder | next to the project file |

## Conversion rules (keep parity with the exporter)

Weights trained from either source must stay interchangeable, so the native reader must do exactly what `Yolo42FolderExporter` does:

- The class id is the position of the name in `get_available_annotations()`, a byte-wise sorted `std::set<std::string>`.
- `x = (rect.x + rect.width/2) / width`, `y = (rect.y + rect.height/2) / height`, `w = rect.width / width`, `h = rect.height / height` (`express_rectangle_data()`).
- The exporter skips records without rects, and records of 0 width or height. It writes val = train.

## The darknet core touch points

Paths are relative to `src/detector-cmake-adaptor/darknet/src/`.

| Where | What it reads |
|---|---|
| `detector/subs/trainer/prepare_train_ctx.c:65`, `prepare_train_ctx()` | `get_paths(net.train_images)` into `ctx->plist` / `ctx->paths`, freed by `clear_context.c:30-32` |
| `data.c:402`, `fill_truth_detection()` | `replace_image_to_label()` + `read_boxes()`. Both `load_data_detection()` variants call it: `:1312` with OpenCV, `:1618` without. Both already hold the decoded size (`ow`/`oh`, `:1163`/`:1521`). `load_thread()` dispatches them (`:1706`) from `load_args` (`darknet.h:187`) |
| `detector.c:545`, `validate_detector_map()` | `get_paths(valid_images)` (`:605`), labels (`:720`), "difficult" labels (`:735`); freed at `:1039-1042`. `calc_map_iteration_ctx()` calls it for `--map` |
| `detector/subs/trainer/calc_map_init_ctx.c:21` | a `fopen(net.valid_images)` existence check |
| `utils.c:259` `replace_image_to_label()`, `data.c:218` `read_boxes()` | the label file lookup itself |

These read labels too but are never reached: `calc_anchors()`, `validate_detector()` and `validate_detector_recall()` have no callers, and `fill_truth_region()` / `fill_truth_swag()` only serve the unused `REGION_DATA` / `SWAG_DATA` load types.

## Routes

**1. Export into a cache, then train.** Either run the DataExporters YOLO4 exporter into a cache folder, or write the lists and labels with DataDrivers alone. Then hand the cache to `Yolo4Folder`. The core stays untouched, but:
- `replace_image_to_label()` needs each label next to its image path. That means copying the images, or linking them (symlinks need privileges on Windows; hard links only work within one file system).
- The exporter regenerates the cfg on every export. That drops cfg edits, and `max_batches` follows the image count, so a resumed training's schedule shifts.
- The cache goes stale whenever the project file changes.
- It adds two non-distribution libraries (exporter variant).

**2. Native dataset source (recommended).** Add a seam to the core with a NULL default, so the YOLO4 folder path doesn't change. Give it the same shape as the existing `detector_context` hooks (`iteration_listener`, `weights_saver`). The images stay where they are and nothing is copied. Route 2 can also do what the export can't:
- normalize records of 0 width/height by the decoded image size;
- detect class drift. Adding a name re-sorts the set, so every later name gets a new id. dxxwjz2 weights embed their class names, so a resume with a mismatch can be refused. The export route trains on the shifted ids without any error;
- check the cfg `[yolo] classes=` against the project's name count before the training starts.

## Proposed design (route 2)

### The C core

```c
// darknet.h: the images of a set and their boxes, borrowed by the core
typedef struct dataset_source
{
  char **paths;
  int count;
  // the boxes of the image, normalized the way read_boxes() reads them;
  // img_w and img_h are of the decoded image
  box_label *(*read_boxes)(const char *path, int img_w, int img_h, int *n,
                           void *data);
  void *data;
} dataset_source;
```

- `detector_context` gets `const dataset_source *train_set` and `*valid_set`. NULL keeps the list files.
- `load_args` gets `const dataset_source *truth`, passed by `load_thread()` → `load_data_detection()` → `fill_truth_detection()`, together with `ow`/`oh`.
- One helper, `box_label *read_truth(const dataset_source *src, const char *path, int img_w, int img_h, int *n)`, replaces each `replace_image_to_label()` + `read_boxes()` pair. It falls back to the label file when `src` is NULL.
- `prepare_train_ctx()` and `validate_detector_map()` copy `paths` into their `list` (`copy_string()`). That way the C side keeps owning what it frees, and neither `clear_context()` nor the frees at `detector.c:1039-1042` change.
- `calc_map_init_ctx()` skips the `fopen()` check when `valid_set` is set.

### The C++ side

Each class goes in its own file pair.

| Class | Where | Role |
|---|---|---|
| `ITrainingSource` | `src/lib/facade/public/` | abstract `valid()`, `path()`, `latest_weights()`, `chart()`, `enter(LibraryContext&)`, pulled out of `Yolo4Folder` |
| `Yolo4Folder` | existing | implements it, with unchanged behavior |
| `AnnotationsProject` | `src/lib/facade/public/`, `src/training/` | implements it for a project file: enters the file's directory and sets `LibraryContext::annotations_project`. Built without DataDrivers, it stays in the API and is never `valid()` |
| `AnnotationsDataset` | `src/darknet-adaptor/` (internal) | loads the project once through DataDrivers, keeps the immutable paths and boxes, owns the two `dataset_source` and the static `read_boxes` callback. `DarknetContext` owns it for the training call |

- `ITrainer::train(const ITrainingSource&)` replaces `train(const Yolo4Folder&)`.
- `LibraryFacade::create_training_source(path)` returns `Yolo4Folder` for a directory and `AnnotationsProject` for a file. The GUI (`TrainingEventsHandler::train()`, which builds the `Yolo4Folder` from the folder entry today) and the CLI stop branching on concrete classes.
- `LibraryContext::annotations_project` is a new field. The training still needs `--train-cfg` (a `.cfg` or a `.dxxwjz2`), so `LibFactory::create_library()` needs no new branch.
- The CLI gets `--annotations-project <FILE>`, wired through `src/app/CMDParamNames.h` → `ApplicationContext` → `src/converters/ApplicationContext2LibraryContext.cpp`. `CommandLineParser` refuses it with `--detect-cfg`.
- The class names come from the dataset and take priority over `--names`, the dxxwjz2 classes and the cfg `names=`. The dxxwjz2 ones are compared to them (class drift).
- The DataDrivers namespace is aliased in one header only: `namespace iadd = ImagesAnnotatorDataDrivers012;`.

### What the dataset checks

- It skips, with a warning, records whose `get_full_path()` isn't a local regular file. That covers web URL records and moved images.
- It refuses a name count that differs from the cfg `classes=`, and dxxwjz2 class names that differ from the project's.
- It warns when a record's `width`/`height` differs from the decoded size (an EXIF rotated or replaced image, which would make the normalization wrong).
- It skips rects of width or height ≤ 0 with a warning. The exporter has no such guard.
- For duplicate full paths, the first one wins, with a warning.

Threading: the dataset is built before the training starts. The loader threads (64, or 6 per GPU with OpenCV) only read it afterwards. DataDrivers isn't thread safe, and it is only touched while loading.

## Build and packaging

- An optional `cmake/enablers/darknetxx-images-annotator-data-drivers-enabler.cmake` with an `ENABLE_IMAGES_ANNOTATOR_PROJECTS` option and `find_package(ImagesAnnotatorDataDrivers-0.12 CONFIG)`. The package name goes in a cache variable, the way ImagesAnnotator's own enablers do it. It is included from `cmake/darknetxx-feature-enabler.cmake` next to the other enablers.
- DataDrivers can only be used installed today:
  - `src/lib/CMakeLists.txt` and `src/log/default-logger/**/CMakeLists.txt` include `${CMAKE_SOURCE_DIR}`, and `cmake/template-project-bootstrap.cmake` / `template-project-feature-enabler.cmake` extend `CMAKE_MODULE_PATH` with `${CMAKE_SOURCE_DIR}/cmake/...`. Under `add_subdirectory()` / FetchContent those resolve to darknetxx;
  - its build tree exports only `facade/public`, so `#include <ImagesAnnotatorDataDrivers-0.12/...>` needs an installation.
- At runtime it needs `libImagesAnnotatorDataDrivers-0.12.so` and libcurl. libcurl is mandatory for DataDrivers, which uses it only in its `UrlHelper` (the de-duplication of web image records).
- Docker, flatpak and snap build DataDrivers inside, as ImagesAnnotator's packagers do. deb, rpm, FreeBSD and MSI either require an installed copy or ship with the feature off.
- Project files name absolute image paths, so the self-contained YOLO4 folder stays the format for the Docker targets (mounted `YOLO4_TRAINING_DATA_DIR`) and for other machines.

## Upstream changes

In DataDrivers (the first two before shipping):

1. Sub-project safe CMake: `PROJECT_SOURCE_DIR` / `PROJECT_BINARY_DIR`, and a build tree that mirrors `include/ImagesAnnotatorDataDrivers-0.12/` the way darknetxx's own `src/lib/CMakeLists.txt` does. This enables a FetchContent fallback.
2. Reject malformed files instead of asserting. `ImageRecordJSON2RecordConverter::convert()` (`rel_path`, `annotations`) and `ImageRecordRectJSON2RecordConverter::convert()` (`name`, `rect`, `x`, `y`, `width`, `height`) only `assert` the keys and then read them through a const `nlohmann::json::operator[]`. On a malformed file, a Debug build aborts and a Release build hits undefined behavior. The folder level keys are validated before, so they're fine.
3. An optional libcurl.

In DataExporters, only if darknetxx is to generate cfgs: a public `Yolov4CfgWriter`.

## Implementation order

1. The C seam with the NULL default, plus a test that the label files still read the same way.
2. `AnnotationsDataset` with its unit and component tests.
3. `ITrainingSource`, `AnnotationsProject`, `LibraryFacade::create_training_source()` and the `LibraryContext` field.
4. The CLI flag and the GUI chooser (a folder or a project file).
5. The CMake enabler, the packagers, and the documentation sections (en_US and uk_UA, plus the README index).

## Tests

- Unit: the box conversion (center and normalization, the 0 size fallback, rects ≤ 0), the class ids, the local file filter.
- Component: a fixture project with a few tiny images, checked against the expected paths and boxes. A parity check confirms the boxes equal the `.txt` files `Yolo42FolderExporter` writes for the same fixture (golden files).
- Component (C): `read_truth()` with a NULL source reads the label files as before.
- Integration: the `src/tests/integration/digits` training, run from a project file instead of the list files.

## Open decisions

1. The cfg of a project-only training: require `--train-cfg`, or generate one (see the DataExporters upstream change).
2. Records without rects: skip them (exporter parity; such an image may just not be annotated yet) or train them as negatives.
3. The validation set: val = train (parity, but an optimistic mAP) or a deterministic split.
4. Web URL records: skip them, or download them (which needs libcurl in darknetxx).
5. Where a project's backup and charts go: next to the project file, or in a `<project>.darknetxx/` folder.
6. The build default of `ENABLE_IMAGES_ANNOTATOR_PROJECTS`.

## Rough size

Route 2 is a few hundred lines plus tests, and the upstream DataDrivers changes 1 and 2. Route 1 needs less code, but brings the library dependencies and copies every image on each export.

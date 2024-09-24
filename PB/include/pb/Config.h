#pragma once

#pragma warning(push)
#pragma warning(disable : 4996)
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#pragma warning(pop)

#include <pb/entities/AspectRatio.h>
#include <pb/entities/PaperSettings.h>

namespace PB {

namespace OneConfig {
static constexpr const char *JPG_EXTENSION = ".jpg";
static constexpr const char *BOOK_EXTENSION = ".photobook";

static constexpr int SMALL_THUMBNAIL_WIDTH = 120;
static constexpr int SMALL_THUMBNAIL_HEIGHT = 80;

static constexpr int MEDIUM_THUMBNAIL_WIDTH = 800;
static constexpr int MEDIUM_THUMBNAIL_HEIGHT = 530;

// The bounding box where the paper size will fit
static constexpr int CANVAS_MIN_MAX_WIDTH = 438;
static constexpr int CANVAS_MIN_MAX_HEIGHT = 310;

static constexpr const char *SMALL_THUMBNAIL_NAME = "thumbnail-small";
static constexpr const char *MEDIUM_THUMBNAIL_NAME = "thumbnail-medium";
static constexpr unsigned    THUMBNAIL_THREADPOOL_THREAD_COUNT = 4;

// Fix separator
static constexpr const char *PHOTO_TIMELINE_DEFAULT_IMAGE =
    "Assets\\coffee-cup.png";
static constexpr const char *LOADING_PHOTO_PLACEHOLDER =
    "Assets\\loading-image.png";

static constexpr const char *VERSION = "0.1.0";

static constexpr PaperSettings A5_LANDSCAPE_PAPER = {PaperType::A5_Landscape,
                                                     300, 2480, 1754};
static constexpr PaperSettings A4_LANDSCAPE_PAPER = {PaperType::A4_Landscape,
                                                     300, 3508, 2480};
static constexpr PaperSettings A3_LANDSCAPE_PAPER = {PaperType::A3_Landscape,
                                                     300, 4920, 3508};

static constexpr PaperSettings A5_PORTRAIT_PAPER = {PaperType::A5_Portrait, 300,
                                                    1754, 2480};
static constexpr PaperSettings A4_PORTRAIT_PAPER = {PaperType::A4_Portrait, 300,
                                                    2480, 3508};
static constexpr PaperSettings A3_PORTRAIT_PAPER = {PaperType::A3_Portrait, 300,
                                                    3508, 4920};
static constexpr PaperSettings SQUARE_PAPER = {PaperType::Square_Paper, 300,
                                               4000, 4000};

static constexpr int MIN_PAPER_WIDTH = 200;
static constexpr int MIN_PAPER_HEIGHT = 200;

static constexpr bool SIMULATE_FEW_HAPPY_WORDS = false;
static constexpr bool SIMULATE_SLOW_MAPPER = false;
static constexpr bool SIMULATE_SLOW_THUMBNAILS_PROCESSOR = false;
static constexpr bool SIMULATE_SLOW_EXPORTER = false;

const std::unordered_map<std::string, PB::AspectRatio> ASPECT_RATIO_PRESET = {
    {"4:3", AspectRatio{4, 3}},
    {"16:9", AspectRatio{16, 9}},
    {"1:1", AspectRatio{1, 1}}};

static constexpr unsigned NUMBER_OF_COLLAGE_NUMBERED_IMAGES = 10;

static constexpr const char *DATABASE_CACHE_TABLE = "CACHE_REGISTER";
static constexpr const char *DATABASE_PROJECT_METADATA_TABLE =
    "PROJECTS_REGISTER";

static constexpr std::array<const char *, 3> DATABASE_CACHE_HEADER = {
    "uuid", "path", "cache_path"};
static constexpr std::array<const char *, 2> DATABASE_PROJECT_METADATA_HEADER =
    {"uuid", "path"};

static constexpr std::pair<const char *,
                           decltype(DATABASE_PROJECT_METADATA_HEADER)>
    DATABASE_PROJECTS_DATA = {DATABASE_PROJECT_METADATA_TABLE,
                              DATABASE_PROJECT_METADATA_HEADER};

static constexpr std::pair<const char *, decltype(DATABASE_CACHE_HEADER)>
    DATABASE_CACHE_DATA = {DATABASE_CACHE_TABLE, DATABASE_CACHE_HEADER};

static constexpr const char *DATABASE_NAME = "databasev2.db";

static constexpr unsigned MAX_HASH_CONFLICTS = 200;

static std::shared_ptr<spdlog::logger> LOGGER = nullptr;

} // namespace OneConfig

} // namespace PB

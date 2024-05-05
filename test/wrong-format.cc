#include "ucpio.h"

#include <filesystem>
#include <gtest/gtest.h>

// find * -print | cpio -ov > test03-wrong_format.cpio
static const auto testfile_03_dir_wrongformat =
  std::filesystem::path(TEST_FILES_DIR) / "test03-wrong_format.cpio";

TEST(CpioRead, WrongFormat)
{
    auto cpio = ucpio_read_new();
    ASSERT_TRUE(cpio != NULL);

    int rc = ucpio_read_open_filename(cpio, testfile_03_dir_wrongformat.u8string().c_str(), 512);
    ASSERT_EQ(rc, uCPIO_OK);

    struct ucpio_entry *ce = NULL;
    rc = ucpio_read_next_header(cpio, &ce);
    ASSERT_EQ(rc, uCPIO_FAIL);

    ASSERT_EQ(ucpio_read_close(cpio), uCPIO_OK);
    ASSERT_EQ(ucpio_read_free(cpio), uCPIO_OK);
}
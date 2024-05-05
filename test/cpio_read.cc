#include "ucpio.h"

#include <archive.h>
#include <archive_entry.h>
#include <filesystem>
#include <gtest/gtest.h>
#include <string>

// ls f-00.txt f-01.txt f-02.txt f-03.txt f-04.txt | cpio -ov -H newc >test01.cpio
static const auto testfile_01 = std::filesystem::path(TEST_FILES_DIR) / "test01.cpio";
// find * -print | cpio -ov -H newc > test02-dir.cpio
static const auto testfile_02_dir = std::filesystem::path(TEST_FILES_DIR) / "test02-dir.cpio";

static const auto testfile_04_symlink =
  std::filesystem::path(TEST_FILES_DIR) / "test04-symlink.cpio";
static const auto testfile_05_hardlink =
  std::filesystem::path(TEST_FILES_DIR) / "test05-hardlink.cpio";

static void cpio_read(const std::filesystem::path &path)
{
    // ---------

    auto archive = archive_read_new();
    ASSERT_TRUE(archive != NULL);

    int rc1 = archive_read_support_format_cpio(archive);
    ASSERT_EQ(rc1, ARCHIVE_OK);

    auto cpio = ucpio_read_new();
    ASSERT_TRUE(cpio != NULL);

    // ---------

    rc1 = archive_read_open_filename(archive, path.u8string().c_str(), 512);
    ASSERT_EQ(rc1, ARCHIVE_OK);

    int rc2 = ucpio_read_open_filename(cpio, path.u8string().c_str(), 512);
    ASSERT_EQ(rc2, uCPIO_OK);

    // ---------
    char b1[512], b2[512];

    struct archive_entry *ae = NULL;
    struct ucpio_entry *ce = NULL;
    while (true) {
        rc1 = archive_read_next_header(archive, &ae);
        ASSERT_NE(rc1, ARCHIVE_FAILED);
        rc2 = ucpio_read_next_header(cpio, &ce);
        ASSERT_NE(rc2, uCPIO_FAIL);

        if (rc1 == ARCHIVE_EOF) {
            ASSERT_TRUE(rc2 == uCPIO_EOF);
            break;
        }

        std::cout << "Testing... " << archive_entry_pathname(ae) << "\n";

        EXPECT_EQ(archive_entry_gid(ae), ucpio_entry_gid(ce)) << archive_entry_gid(ae);
        EXPECT_EQ(archive_entry_uid(ae), ucpio_entry_uid(ce));
        EXPECT_EQ(archive_entry_gid(ae), ucpio_entry_gid(ce));
        EXPECT_EQ(archive_entry_mode(ae), ucpio_entry_mode(ce));
        EXPECT_EQ(archive_entry_size(ae), ucpio_entry_size(ce));
        EXPECT_EQ(archive_entry_devmajor(ae), ucpio_entry_devmajor(ce));
        EXPECT_EQ(archive_entry_devminor(ae), ucpio_entry_devminor(ce));
        EXPECT_EQ(archive_entry_ino(ae), ucpio_entry_ino(ce));
        EXPECT_EQ(archive_entry_rdevmajor(ae), ucpio_entry_rdevmajor(ce));
        EXPECT_EQ(archive_entry_rdevminor(ae), ucpio_entry_rdevminor(ce));
        EXPECT_EQ(archive_entry_nlink(ae), ucpio_entry_nlink(ce));
        EXPECT_EQ(archive_entry_mtime(ae), ucpio_entry_mtime(ce));
        EXPECT_STREQ(archive_entry_pathname(ae), ucpio_entry_pathname(ce));
        EXPECT_STREQ(archive_entry_hardlink(ae), ucpio_entry_hardlink(ce));
        EXPECT_STREQ(archive_entry_symlink(ae), ucpio_entry_symlink(ce));

        for (;;) {
            ssize_t rc1 = archive_read_data(archive, b1, sizeof(b1));
            ssize_t rc2 = ucpio_read_data(cpio, b2, sizeof(b2));
            ASSERT_EQ(rc1, rc2) << archive_entry_pathname(ae);
            if (rc1 == 0) {
                break;
            }

            for (int i = 0; i < rc1; ++i) {
                EXPECT_EQ(b1[i], b2[i]) << "data comparison -> " << i;
            }
        }

        // ASSERT_EQ(archive_read_data_skip(archive), ARCHIVE_OK);
        // ASSERT_EQ(ucpio_read_data_skip(cpio), uCPIO_OK);
    }

    // ---------

    ASSERT_EQ(archive_read_close(archive), ARCHIVE_OK);
    ASSERT_EQ(ucpio_read_close(cpio), uCPIO_OK);

    // ---------

    ASSERT_EQ(archive_read_free(archive), ARCHIVE_OK);
    ASSERT_EQ(ucpio_read_free(cpio), uCPIO_OK);
}

TEST(CpioRead, RegularFiles)
{
    cpio_read(testfile_01);
}

TEST(CpioRead, Directory)
{
    cpio_read(testfile_02_dir);
}

TEST(CpioRead, Symlink)
{
    cpio_read(testfile_04_symlink);
}
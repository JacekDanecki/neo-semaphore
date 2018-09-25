/*
 * Copyright (C) 2017-2018 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "unit_tests/gen_common/gen_cmd_parse.h"
#include "unit_tests/fixtures/device_fixture.h"
#include "test.h"

using namespace OCLRT;

struct CommandParse
    : public DeviceFixture,
      public ::testing::Test {
    void SetUp() override {
        DeviceFixture::SetUp();
    }

    void TearDown() override {
        DeviceFixture::TearDown();
    }
};

HWTEST_F(CommandParse, parseCommandBufferWithNULLBuffer) {
    typedef typename FamilyType::PARSE PARSE;
    GenCmdList cmds;
    EXPECT_FALSE(PARSE::parseCommandBuffer(cmds, nullptr, sizeof(void *)));
}

HWTEST_F(CommandParse, parseCommandBufferWithGarbage) {
    typedef typename FamilyType::PARSE PARSE;
    uint32_t buffer = 0xbaadf00d;
    GenCmdList cmds;

    EXPECT_FALSE(PARSE::parseCommandBuffer(cmds, &buffer, sizeof(buffer)));
}

HWTEST_F(CommandParse, getCommandLengthWithGarbage) {
    typedef typename FamilyType::PARSE PARSE;
    uint32_t buffer = 0xbaadf00d;

    EXPECT_EQ(0u, PARSE::getCommandLength(&buffer));
}

HWCMDTEST_F(IGFX_GEN8_CORE, CommandParse, parseCommandBufferWithLength) {
    typedef typename FamilyType::PARSE PARSE;
    typedef typename FamilyType::GPGPU_WALKER GPGPU_WALKER;
    GenCmdList cmds;
    GPGPU_WALKER buffer = GPGPU_WALKER::sInit();

    EXPECT_TRUE(PARSE::parseCommandBuffer(cmds, &buffer, 0));
    EXPECT_FALSE(PARSE::parseCommandBuffer(cmds, &buffer, 1));
    EXPECT_FALSE(PARSE::parseCommandBuffer(cmds, &buffer, 2));
    EXPECT_FALSE(PARSE::parseCommandBuffer(cmds, &buffer, 3));
    EXPECT_FALSE(PARSE::parseCommandBuffer(cmds, &buffer, 4));
    EXPECT_TRUE(PARSE::parseCommandBuffer(cmds, &buffer, sizeof(buffer)));
}

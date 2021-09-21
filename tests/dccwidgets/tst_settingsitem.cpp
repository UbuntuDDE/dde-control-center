#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/settingsitem.h"

using namespace dcc::widgets;

class Tst_SettingsItem : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new SettingsItem();
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    SettingsItem *obj = nullptr;
};

TEST_F(Tst_SettingsItem, coverage)
{
    obj->setIsErr(0);
    obj->isErr();
    obj->addBackground();
}

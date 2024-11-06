/**
 * MIT License
 *
 * Copyright (C) 2023 Huawei Device Co., Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef HARMONY_PAGERVIEW_SRC_MAIN_CPP_VIEWPAGERPACKAGE_H
#define HARMONY_PAGERVIEW_SRC_MAIN_CPP_VIEWPAGERPACKAGE_H
#include "ViewPagerComponentInstance.h"
#include "RNCViewPagerTurbomodule.h"
#include "generated/RNOH/generated/BaseReactNativePagerViewPackage.h"
using namespace rnoh;
using namespace facebook;
namespace rnoh {

class RNCViewPagerContextFactoryDelegate : public TurboModuleFactoryDelegate {
public:
    SharedTurboModule createTurboModule(Context ctx, const std::string &name) const override {
        if (name == "RNCViewPagerContext") {
            return std::make_shared<RNCViewPagerTurbomodule>(ctx, name);
        }
        return nullptr;
    };
};

class ViewPagerPackage : public BaseReactNativePagerViewPackage {
    using Super = BaseReactNativePagerViewPackage;

public:
    ViewPagerPackage(Package::Context ctx) : Super(ctx) {}

    ComponentInstance::Shared createComponentInstance(const ComponentInstance::Context &ctx) {
        if (ctx.componentName == "RNCViewPager") {
            return std::make_shared<ViewPagerComponentInstance>(ctx);
        }
        return nullptr;
    };

    std::unique_ptr<TurboModuleFactoryDelegate> createTurboModuleFactoryDelegate() override {
        return std::make_unique<RNCViewPagerContextFactoryDelegate>();
    }
};
} // namespace rnoh
#endif
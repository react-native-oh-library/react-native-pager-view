/**
 * MIT License
 *
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#pragma once
#include "RNOH/CppComponentInstance.h"
#include "SwiperNode.h"
#include "EventEmitters.h"
#include <folly/dynamic.h>
#include<math.h>
#include "ShadowNodes.h"

namespace rnoh {
    class ViewPagerComponentInstance : public CppComponentInstance<facebook::react::RNCViewPagerShadowNode>, public SwiperNodeDelegate {
    private:
        SwiperNode m_swiperNode;
    
        double m_pageIndex;
    
        bool m_scrollEnabled = false;
    
        bool m_nativeLock = false;

        std::string m_keyboardDismissMode;
    
        bool m_needSetProps = true;
    
        bool m_gestureStatus = false;

        bool m_clickTap = false;

        bool m_gestureSwipe = false;

        struct PanActionCallBack {
            SwiperNodeDelegate *swiperNodeDelegate;
        };

    public:
        ViewPagerComponentInstance(Context context);

        void onChildInserted(ComponentInstance::Shared const &childComponentInstance, std::size_t index) override;

        void onChildRemoved(ComponentInstance::Shared const &childComponentInstance) override;

        SwiperNode &getLocalRootArkUINode() override;

        void onPropsChanged(SharedConcreteProps const &props) override;
    
        void onPageSelected(double pageIndex) override;
    
        void onPageScroll(facebook::react::RNCViewPagerEventEmitter::OnPageScroll pageScroll) override;
    
        void onPageScrollStateChanged(facebook::react::RNCViewPagerEventEmitter::OnPageScrollStateChanged value) override;

        void handleCommand(std::string const &commandName, folly::dynamic const &args) override;
    
        std::vector<TouchTarget::Shared> getTouchTargetChildren() override;
    
        bool getScrollEnabled() override;
    
        bool getNativeLock() override;
    
        void setKeyboardDismiss() override;
    
       void onNativeResponderBlockChange(bool isBlocked) override;
    
       void setGestureStatus(bool gestureStatus) override;
    
       bool isHandlingTouches() const override;

       bool getClickTap() override;

       void setClickTap(bool clickTap) override;
    
       void regsiterGestureEvent();
     };
} // namespace rnoh
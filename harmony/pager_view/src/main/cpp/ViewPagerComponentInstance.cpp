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

#include "ViewPagerComponentInstance.h"
#include "Props.h"
#include <arkui/native_gesture.h>
#include <arkui/native_interface.h>

namespace rnoh {

    ViewPagerComponentInstance::ViewPagerComponentInstance(Context context)
        : CppComponentInstance(std::move(context)) {
        m_swiperNode.setSwiperNodeDelegate(this);
        this->regsiterGestureEvent();
    }

    void ViewPagerComponentInstance::onChildInserted(ComponentInstance::Shared const &childComponentInstance, std::size_t index) {
        CppComponentInstance::onChildInserted(childComponentInstance, index);
        m_swiperNode.insertChild(childComponentInstance->getLocalRootArkUINode(), index);
    }

    SwiperNode &ViewPagerComponentInstance::getLocalRootArkUINode() { return m_swiperNode; }

    void ViewPagerComponentInstance::onChildRemoved(ComponentInstance::Shared const &childComponentInstance) {
        CppComponentInstance::onChildRemoved(childComponentInstance);
        m_swiperNode.removeChild(childComponentInstance->getLocalRootArkUINode());
    };

    void ViewPagerComponentInstance::onPropsChanged(SharedConcreteProps const &props) {
        CppComponentInstance::onPropsChanged(props);
         DLOG(INFO) << "ViewPagerComponentInstance::m_needSetProps: " << this->m_needSetProps << " initialPage:" << props->initialPage;
         if(!this->m_needSetProps){
             this->m_needSetProps=true;
             return;
         }
         this->m_scrollEnabled = props->scrollEnabled;
         this->m_pageIndex = props->initialPage;
         this->m_keyboardDismissMode = props->keyboardDismissMode;
         this->getLocalRootArkUINode().setIndex(props->initialPage);
         this->getLocalRootArkUINode().setVertical(props->orientation);
         this->getLocalRootArkUINode().setDirection(props->layoutDirection);
         this->getLocalRootArkUINode().setItemSpace(props->pageMargin);
         this->getLocalRootArkUINode().setDisableSwipe(!props->scrollEnabled);
         this->getLocalRootArkUINode().setLoop(false);
         this->getLocalRootArkUINode().setIndicator(false);
         this->getLocalRootArkUINode().setCachedCount(props->offscreenPageLimit);
         this->getLocalRootArkUINode().setCurve(ARKUI_CURVE_SMOOTH);
         this->getLocalRootArkUINode().setDuration(400); 
         if(props->overdrag){
               this->getLocalRootArkUINode().setEffectMode("SPRING");
         }
         else{
             if(props->overScrollMode == "never"){
                   this->getLocalRootArkUINode().setEffectMode("NONE");
             }
             else{
                   this->getLocalRootArkUINode().setEffectMode("FADE");                 
             }
         }
    }

    void ViewPagerComponentInstance::onPageSelected(double pageIndex) {
        this->m_pageIndex = pageIndex;
        DLOG(INFO) << "ViewPagerComponentInstance::onPageSelected:" << pageIndex ;
        facebook::react::RNCViewPagerEventEmitter::OnPageSelected event = {pageIndex};
        m_eventEmitter->onPageSelected(event); 
    }

    void ViewPagerComponentInstance::onPageScroll(facebook::react::RNCViewPagerEventEmitter::OnPageScroll pageScroll) {
        DLOG(INFO) << "ViewPagerComponentInstance::onPageScroll" ;
        m_eventEmitter->onPageScroll(pageScroll);
    }

    void ViewPagerComponentInstance::onPageScrollStateChanged(facebook::react::RNCViewPagerEventEmitter::OnPageScrollStateChanged value) {
        DLOG(INFO) << "ViewPagerComponentInstance::onPageScrollStateChanged" ;
        m_eventEmitter->onPageScrollStateChanged(value);
    }

    void ViewPagerComponentInstance::handleCommand(std::string const &commandName, folly::dynamic const &args) {
        DLOG(INFO) << "handleCommand-->commandName: " << commandName;
        if (commandName == "setScrollEnabledImperatively" && args.isArray() && args.size() == 1) {
             DLOG(INFO) << "handleCommand-->setScrollEnabledImperatively: " << args[0];
             this->m_scrollEnabled = args[0].asBool();
             this->getLocalRootArkUINode().setDisableSwipe(!args[0].asBool());
             if (!args[0].asBool()) {
                   facebook::react::RNCViewPagerEventEmitter::OnPageScrollStateChanged event = {
                       facebook::react::RNCViewPagerEventEmitter::OnPageScrollStateChangedPageScrollState::Idle};
                   m_eventEmitter->onPageScrollStateChanged(event);
             }
        } else if (commandName == "setPage" && args.isArray() && args.size() == 1) {
             this->m_clickTap = true;
             DLOG(INFO) << "handleCommand-->setPage: " << args[0];
             this->m_needSetProps = false;
             this->getLocalRootArkUINode().setIndex(args[0].asInt());
        } else if (commandName == "setPageWithoutAnimation" && args.isArray() && args.size() == 1) {
             DLOG(INFO) << "handleCommand-->setPageWithoutAnimation: " << args[0];
             this->m_needSetProps = false;
             this->m_clickTap = true;
             this->getLocalRootArkUINode().setIndexNoAnimation(args[0].asInt());
        }
    }

    bool ViewPagerComponentInstance::getScrollEnabled() {
       return this->m_scrollEnabled;
    }

    bool ViewPagerComponentInstance::getNativeLock() {
       return this->m_nativeLock;
    }

    std::vector<TouchTarget::Shared> ViewPagerComponentInstance::getTouchTargetChildren() {
        auto children = getChildren();
        std::vector<TouchTarget::Shared> result(1);
        result.push_back(children.at(m_pageIndex));
        return result;
    }

    int ViewPagerComponentInstance::getLayoutMetricsWidth(){
       return this->m_layoutMetrics.frame.size.width;
    }

    void ViewPagerComponentInstance::setKeyboardDismiss() {
        DLOG(INFO) << "ViewPagerComponentInstance::setKeyboardDismiss: " << this->m_keyboardDismissMode ;
        if (this->m_keyboardDismissMode != "on-drag") return;
        auto rnInstancePtr =this->m_deps->rnInstance.lock();
        if (rnInstancePtr != nullptr) {
            auto turboModule = rnInstancePtr->getTurboModule("RNCViewPagerContext");
            auto arkTsTurboModule = std::dynamic_pointer_cast<rnoh::ArkTSTurboModule>(turboModule);
            arkTsTurboModule->callSync("keyboardDismiss", {});
        }
    }

    void ViewPagerComponentInstance::onNativeResponderBlockChange(bool isBlocked) {
        DLOG(INFO) << "ViewPagerComponentInstance::setNativeResponderBlocked:" << isBlocked;
        if(isBlocked){
            this->m_nativeLock = true;
            this->getLocalRootArkUINode().setDisableSwipe(true);
            facebook::react::RNCViewPagerEventEmitter::OnPageScrollStateChanged event = {
              facebook::react::RNCViewPagerEventEmitter::OnPageScrollStateChangedPageScrollState::Idle};
            m_eventEmitter->onPageScrollStateChanged(event);
        }
        else{
            this->m_nativeLock = false;
            this->getLocalRootArkUINode().setDisableSwipe(false);
        }
    }

    void ViewPagerComponentInstance::setGestureStatus(bool gestureStatus) {
        this->m_gestureStatus = gestureStatus;
        DLOG(INFO) << "ViewPagerComponentInstance::setGestureStatus:" << gestureStatus;
    }

    bool ViewPagerComponentInstance::isHandlingTouches() const {
        DLOG(INFO) << "ViewPagerComponentInstance::isHandlingTouches:" << this->m_gestureStatus;
        return this->m_gestureStatus ? true : false;
    }

    void ViewPagerComponentInstance::regsiterGestureEvent() {
        DLOG(INFO) << "ViewPagerComponentInstance::regsiterGestureEvent";
        auto gestureApi = OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_GESTURE, "ArkUI_NativeGestureAPI_1");
        auto swiperGestureApi = reinterpret_cast<ArkUI_NativeGestureAPI_1 *>(gestureApi);
        auto swiperGestureRecognizer = swiperGestureApi->createPanGesture(1, GESTURE_DIRECTION_ALL, 50);
        auto onPanActionCallBack = [](ArkUI_GestureEvent *event, void *extraParam) {
            PanActionCallBack *panActionCallBack = (PanActionCallBack *)extraParam;
            ArkUI_GestureEventActionType actionType = OH_ArkUI_GestureEvent_GetActionType(event);

            if (actionType == GESTURE_EVENT_ACTION_UPDATE) {
                DLOG(INFO) << "ViewPagerComponentInstance::regsiterGestureEvent GESTURE_EVENT_ACTION_UPDATE";
                panActionCallBack->swiperNodeDelegate->setGestureStatus(true);
            } else if (actionType == GESTURE_EVENT_ACTION_END) {
                DLOG(INFO) << "ViewPagerComponentInstance::regsiterGestureEvent GESTURE_EVENT_ACTION_END";
                panActionCallBack->swiperNodeDelegate->setGestureStatus(false);
            }
        };
        PanActionCallBack *panActionCallBack = new PanActionCallBack{
            .swiperNodeDelegate = this,
        };
        swiperGestureApi->setGestureEventTarget(swiperGestureRecognizer,
                                                GESTURE_EVENT_ACTION_ACCEPT | GESTURE_EVENT_ACTION_UPDATE |
                                                    GESTURE_EVENT_ACTION_END,
                                                panActionCallBack, onPanActionCallBack);
        swiperGestureApi->addGestureToNode(this->getLocalRootArkUINode().getArkUINodeHandle(), swiperGestureRecognizer,
                                           PARALLEL, NORMAL_GESTURE_MASK);
    }

    bool ViewPagerComponentInstance::getClickTap() { return this->m_clickTap; }

    void ViewPagerComponentInstance::setClickTap(bool clickTap) { this->m_clickTap = clickTap; }


} // namespace rnoh/

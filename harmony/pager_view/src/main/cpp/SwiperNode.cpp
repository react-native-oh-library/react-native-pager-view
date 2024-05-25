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

#include "SwiperNode.h"
#include <memory>
#include "RNOH/arkui/NativeNodeApi.h"
#include "glog/logging.h"

namespace rnoh {

    SwiperNode::SwiperNode()
        : ArkUINode(NativeNodeApi::getInstance()->createNode(ArkUI_NodeType::ARKUI_NODE_SWIPER)),
          m_stackArkUINodeHandle(nullptr),m_swiperNodeDelegate(nullptr) {
        maybeThrow(NativeNodeApi::getInstance()->registerNodeEvent(m_nodeHandle, NODE_SWIPER_EVENT_ON_ANIMATION_START, NODE_SWIPER_EVENT_ON_ANIMATION_START,this));
        maybeThrow(NativeNodeApi::getInstance()->registerNodeEvent(m_nodeHandle, NODE_SWIPER_EVENT_ON_ANIMATION_END,NODE_SWIPER_EVENT_ON_ANIMATION_END, this));
        maybeThrow(NativeNodeApi::getInstance()->registerNodeEvent(m_nodeHandle, NODE_SWIPER_EVENT_ON_CHANGE, NODE_SWIPER_EVENT_ON_CHANGE,this));
        maybeThrow(NativeNodeApi::getInstance()->registerNodeEvent(m_nodeHandle, NODE_SWIPER_EVENT_ON_CONTENT_DID_SCROLL, NODE_SWIPER_EVENT_ON_CONTENT_DID_SCROLL,this));
    }


   SwiperNode::~SwiperNode() {
        NativeNodeApi::getInstance()->unregisterNodeEvent(m_nodeHandle, NODE_SWIPER_EVENT_ON_ANIMATION_START);
        NativeNodeApi::getInstance()->unregisterNodeEvent(m_nodeHandle, NODE_SWIPER_EVENT_ON_ANIMATION_END);
        NativeNodeApi::getInstance()->unregisterNodeEvent(m_nodeHandle, NODE_SWIPER_EVENT_ON_CHANGE);
        NativeNodeApi::getInstance()->unregisterNodeEvent(m_nodeHandle, NODE_SWIPER_EVENT_ON_CONTENT_DID_SCROLL);
    }


    void SwiperNode::insertChild(ArkUINode &child, std::size_t index) {
        m_stackArkUINodeHandle = NativeNodeApi::getInstance()->createNode(ArkUI_NodeType::ARKUI_NODE_STACK);
        NativeNodeApi::getInstance()->addChild(m_nodeHandle, m_stackArkUINodeHandle);
        NativeNodeApi::getInstance()->insertChildAt(m_stackArkUINodeHandle, child.getArkUINodeHandle(),index);
    }

    void SwiperNode::removeChild(ArkUINode &child) {
        maybeThrow(NativeNodeApi::getInstance()->removeChild(m_stackArkUINodeHandle, child.getArkUINodeHandle()));
    }

    void SwiperNode::setSwiperNodeDelegate(SwiperNodeDelegate *swiperNodeDelegate) {
        m_swiperNodeDelegate = swiperNodeDelegate;
    }

    void SwiperNode::onNodeEvent(ArkUI_NodeEventType eventType, EventArgs& eventArgs) {
        if (m_swiperNodeDelegate == nullptr) return; 
        LOG(INFO) << "onNodeEvent-->start";
        if (eventType == ArkUI_NodeEventType::NODE_SWIPER_EVENT_ON_ANIMATION_START) {
              this->m_targetIndex = eventArgs[1].i32;
               LOG(INFO) << "onNodeEvent-->NODE_SWIPER_EVENT_ON_ANIMATION_START m_currentIndex:"<< eventArgs[0].i32 << " m_targetIndex:" << this->m_targetIndex;
               m_swiperNodeDelegate->setKeyboardDismiss();
               facebook::react::RNCViewPagerEventEmitter::OnPageScrollStateChanged event = {
                    facebook::react::RNCViewPagerEventEmitter::OnPageScrollStateChangedPageScrollState::Settling};
               m_swiperNodeDelegate->onPageScrollStateChanged(event);
        }
        else if (eventType == ArkUI_NodeEventType::NODE_SWIPER_EVENT_ON_ANIMATION_END) {
                LOG(INFO) << "onNodeEvent-->NODE_SWIPER_EVENT_ON_ANIMATION_END: " << eventArgs[0].i32;
                facebook::react::RNCViewPagerEventEmitter::OnPageScroll m_onPageScroll = {
                      static_cast<double>(eventArgs[0].i32),0};
                m_swiperNodeDelegate->onPageScroll(m_onPageScroll);
                facebook::react::RNCViewPagerEventEmitter::OnPageScrollStateChanged pageScrollStateChange = {
                      facebook::react::RNCViewPagerEventEmitter::OnPageScrollStateChangedPageScrollState::Idle};   
                m_swiperNodeDelegate->onPageScrollStateChanged(pageScrollStateChange);
        }
        else if (eventType == ArkUI_NodeEventType::NODE_SWIPER_EVENT_ON_CHANGE) {
                LOG(INFO) << "onNodeEvent-->NODE_SWIPER_EVENT_ON_CHANGE: " << eventArgs[0].i32;
                m_swiperNodeDelegate->onPageSelected(eventArgs[0].i32);
                facebook::react::RNCViewPagerEventEmitter::OnPageScroll m_onPageScroll = {
                      static_cast<double>(eventArgs[0].i32),0};
                m_swiperNodeDelegate->onPageScroll(m_onPageScroll);
        }
        else if (eventType == ArkUI_NodeEventType::NODE_SWIPER_EVENT_ON_CONTENT_DID_SCROLL) {
               LOG(INFO) << "onNodeEvent-->NODE_SWIPER_EVENT_ON_CONTENT_DID_SCROLL getScrollEnabled:" 
                 << m_swiperNodeDelegate->getScrollEnabled() << " getNativeLock:"<< m_swiperNodeDelegate->getNativeLock();
               if(!m_swiperNodeDelegate->getScrollEnabled() || m_swiperNodeDelegate->getNativeLock()) return;
               LOG(INFO) << "onNodeEvent-->NODE_SWIPER_EVENT_ON_CONTENT_DID_SCROLL selectedIndex:" << eventArgs[0].i32 << " index:" 
                 << eventArgs[1].i32 << "position:" << eventArgs[2].f32;
               double selectedIndex = eventArgs[0].i32;
               double index = eventArgs[1].i32;
               if(selectedIndex != index){
                  double offset;
                  if((selectedIndex > index) && ((selectedIndex - index) == 1)){
                      offset = -eventArgs[2].f32;
                      if(selectedIndex >= 1){
                        selectedIndex=selectedIndex-1;
                      }
                      if(offset < 0.0202){
                        m_swiperNodeDelegate->onPageSelected(this->m_targetIndex);
                      }
                      if(offset < 0){
                         offset = 0;
                      }
                     LOG(INFO) << "onNodeEvent-->NODE_SWIPER_EVENT_ON_CONTENT_DID_SCROLL right selectedIndex:" << selectedIndex << " offset:" << offset;
                      facebook::react::RNCViewPagerEventEmitter::OnPageScroll m_onPageScroll = {
                       selectedIndex,offset};                    
                      m_swiperNodeDelegate->onPageScroll(m_onPageScroll);
                  }
                  else if((index - selectedIndex) == 1){
                       if(eventArgs[2].f32 <= 0){
                          eventArgs[2].f32 = 0;
                       }
                       offset = 1 - eventArgs[2].f32;
                       if(offset > 0.968){
                          m_swiperNodeDelegate->onPageSelected(this->m_targetIndex);
                       }
                       if(offset > 1){
                          offset = 1;
                       }
                      LOG(INFO) << "onNodeEvent-->NODE_SWIPER_EVENT_ON_CONTENT_DID_SCROLL left selectedIndex:" << selectedIndex << " offset:" << offset ;
                      facebook::react::RNCViewPagerEventEmitter::OnPageScroll m_onPageScroll = {
                       selectedIndex,offset};                    
                      m_swiperNodeDelegate->onPageScroll(m_onPageScroll);
                  }
             }
           
        } 
    }

    SwiperNode &SwiperNode::setIndex(int const &initialPage) {
        ArkUI_NumberValue indexValue[] = {{.i32 = initialPage}};
        ArkUI_AttributeItem indexItem = {indexValue, sizeof(indexValue) / sizeof(ArkUI_NumberValue)};
        maybeThrow(NativeNodeApi::getInstance()->setAttribute(m_nodeHandle, NODE_SWIPER_INDEX, &indexItem));
        return *this;
    }

    SwiperNode &SwiperNode::setVertical(std::string const &orientation) { 
        if(orientation.empty()) return *this;
        int orientationValue = (orientation == "horizontal") ? 0:1;
        ArkUI_NumberValue indexValue[] = {{.i32 = orientationValue}};
        ArkUI_AttributeItem item = {indexValue, sizeof(indexValue) / sizeof(ArkUI_NumberValue)};
        maybeThrow(NativeNodeApi::getInstance()->setAttribute(m_nodeHandle, NODE_SWIPER_VERTICAL, &item));
        return *this;
    }

    SwiperNode &SwiperNode::setDirection(std::string const &layoutDirection) {
        if(layoutDirection.empty()) return *this;
        int  layoutDirectionValue = (layoutDirection == "ltr") ? 1:0;
        ArkUI_NumberValue indexValue[] = {{.i32 = layoutDirectionValue}};
        ArkUI_AttributeItem indexItem = {indexValue, sizeof(indexValue) / sizeof(ArkUI_NumberValue)};
        maybeThrow(NativeNodeApi::getInstance()->setAttribute(m_nodeHandle, NODE_DIRECTION,&indexItem));
        return *this;
    }

   SwiperNode &SwiperNode::setItemSpace(float  const &pageMargin) { 
        ArkUI_NumberValue indexValue[] = {{.f32 = pageMargin}};
        ArkUI_AttributeItem indexItem = {indexValue, sizeof(indexValue) / sizeof(ArkUI_NumberValue)};
        maybeThrow(NativeNodeApi::getInstance()->setAttribute(m_nodeHandle, NODE_SWIPER_ITEM_SPACE,&indexItem));
        return *this;
    }

   SwiperNode &SwiperNode::setCachedCount(int const &offscreenPageLimit) { 
        ArkUI_NumberValue indexValue[] = {{.i32 = offscreenPageLimit}};
        ArkUI_AttributeItem indexItem = {indexValue, sizeof(indexValue) / sizeof(ArkUI_NumberValue)};
        maybeThrow(NativeNodeApi::getInstance()->setAttribute(m_nodeHandle, NODE_SWIPER_CACHED_COUNT,&indexItem));
        return *this;    
    }

   SwiperNode &SwiperNode::setDisableSwipe(bool const &disableSwipe) {
        int disableSwipeValue = disableSwipe ? 1:0;
        ArkUI_NumberValue indexValue[] = {{.i32 = disableSwipeValue}};
        ArkUI_AttributeItem indexItem = {indexValue, sizeof(indexValue) / sizeof(ArkUI_NumberValue)};
        maybeThrow(NativeNodeApi::getInstance()->setAttribute(m_nodeHandle, NODE_SWIPER_DISABLE_SWIPE,&indexItem));
        return *this;
    }

   SwiperNode &SwiperNode::setEffectMode(std::string const &effectMode) {
        if(effectMode.empty()) return *this;  
        if(effectMode == "SPRING"){
           ArkUI_NumberValue indexValue[] = {{.i32 = 0}};
            ArkUI_AttributeItem indexItem = {indexValue, sizeof(indexValue) / sizeof(ArkUI_NumberValue)};
            maybeThrow(NativeNodeApi::getInstance()->setAttribute(m_nodeHandle, NODE_SWIPER_EDGE_EFFECT_MODE,&indexItem));           
        }
        else if(effectMode == "NONE"){
           ArkUI_NumberValue  indexValue[] = {{.i32 = 2}};
            ArkUI_AttributeItem indexItem = {indexValue, sizeof(indexValue) / sizeof(ArkUI_NumberValue)};
            maybeThrow(NativeNodeApi::getInstance()->setAttribute(m_nodeHandle, NODE_SWIPER_EDGE_EFFECT_MODE,&indexItem));               
        }     
        else{
            ArkUI_NumberValue indexValue[] = {{.i32 = 1}};
            ArkUI_AttributeItem indexItem = {indexValue, sizeof(indexValue) / sizeof(ArkUI_NumberValue)};
            maybeThrow(NativeNodeApi::getInstance()->setAttribute(m_nodeHandle, NODE_SWIPER_EDGE_EFFECT_MODE,&indexItem));            
        }
        return *this;
    }

   SwiperNode &SwiperNode::setLoop(bool const &loop) {
        int loopValue = loop ? 1:0;
        ArkUI_NumberValue indexValue[] = {{.i32 = loopValue}};
        ArkUI_AttributeItem indexItem = {indexValue, sizeof(indexValue) / sizeof(ArkUI_NumberValue)};
        maybeThrow(NativeNodeApi::getInstance()->setAttribute(m_nodeHandle, NODE_SWIPER_LOOP,&indexItem));
        return *this;
    }

   SwiperNode &SwiperNode::setIndicator(bool const &indicator) {
        int indicatorValue = indicator ? 1:0;
        ArkUI_NumberValue indexValue[] = {{.i32 = indicatorValue}};
        ArkUI_AttributeItem indexItem = {indexValue, sizeof(indexValue) / sizeof(ArkUI_NumberValue)};
        maybeThrow(NativeNodeApi::getInstance()->setAttribute(m_nodeHandle, NODE_SWIPER_SHOW_INDICATOR,&indexItem));
        return *this;
    }

   SwiperNode &SwiperNode::setDuration(int const &duration) { 
        ArkUI_NumberValue indexValue[] = {{.i32 = duration}};
        ArkUI_AttributeItem indexItem = {indexValue, sizeof(indexValue) / sizeof(ArkUI_NumberValue)};
        maybeThrow(NativeNodeApi::getInstance()->setAttribute(m_nodeHandle, NODE_SWIPER_DURATION,&indexItem));
        return *this;
    }

    SwiperNode &SwiperNode::setCurve(int const &curve) {
        ArkUI_NumberValue indexValue[] = {{.i32 = curve}};
        ArkUI_AttributeItem indexItem = {indexValue, sizeof(indexValue) / sizeof(ArkUI_NumberValue)};
        maybeThrow(NativeNodeApi::getInstance()->setAttribute(m_nodeHandle, NODE_SWIPER_CURVE, &indexItem));
        return *this;
    }

} // namespace rnoh

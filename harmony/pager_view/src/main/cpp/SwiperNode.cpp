#include "SwiperNode.h"
#include <memory>
#include "RNOH/arkui/NativeNodeApi.h"
#include "glog/logging.h"

namespace rnoh {

    SwiperNode::SwiperNode()
        : ArkUINode(NativeNodeApi::getInstance()->createNode(ArkUI_NodeType::ARKUI_NODE_SWIPER)),
          m_swiperNodeDelegate(nullptr) {
        maybeThrow(NativeNodeApi::getInstance()->registerNodeEvent(m_nodeHandle, NODE_SWIPER_EVENT_ON_ANIMATION_START, 0));
        maybeThrow(NativeNodeApi::getInstance()->registerNodeEvent(m_nodeHandle, NODE_SWIPER_EVENT_ON_ANIMATION_END, 1));
        maybeThrow(NativeNodeApi::getInstance()->registerNodeEvent(m_nodeHandle, NODE_SWIPER_EVENT_ON_CHANGE, 2));
        maybeThrow(NativeNodeApi::getInstance()->registerNodeEvent(m_nodeHandle, NODE_SWIPER_EVENT_ON_GESTURE_SWIPE, 3));
    }


   SwiperNode::~SwiperNode() {
        NativeNodeApi::getInstance()->unregisterNodeEvent(m_nodeHandle, NODE_SWIPER_EVENT_ON_ANIMATION_START);
        NativeNodeApi::getInstance()->unregisterNodeEvent(m_nodeHandle, NODE_SWIPER_EVENT_ON_ANIMATION_END);
        NativeNodeApi::getInstance()->unregisterNodeEvent(m_nodeHandle, NODE_SWIPER_EVENT_ON_CHANGE);
        NativeNodeApi::getInstance()->unregisterNodeEvent(m_nodeHandle, NODE_SWIPER_EVENT_ON_GESTURE_SWIPE);
    }


    void SwiperNode::insertChild(ArkUINode &child, std::size_t index) {
        maybeThrow(NativeNodeApi::getInstance()->insertChildAt(m_nodeHandle, child.getArkUINodeHandle(),index));                                  
    }

    void SwiperNode::removeChild(ArkUINode &child) {
        maybeThrow(NativeNodeApi::getInstance()->removeChild(m_nodeHandle, child.getArkUINodeHandle()));
    }

    void SwiperNode::setSwiperNodeDelegate(SwiperNodeDelegate *swiperNodeDelegate) {
        m_swiperNodeDelegate = swiperNodeDelegate;
    }

    void SwiperNode::onNodeEvent(ArkUI_NodeEvent *event) {
        if (m_swiperNodeDelegate == nullptr) return; 
        LOG(INFO) << "onNodeEvent-->start";
        if (event->kind == ArkUI_NodeEventType::NODE_SWIPER_EVENT_ON_ANIMATION_START) {
               LOG(INFO) << "onNodeEvent-->NODE_SWIPER_EVENT_ON_ANIMATION_START";
               facebook::react::RNCViewPagerEventEmitter::OnPageScrollStateChanged event = {
                    facebook::react::RNCViewPagerEventEmitter::OnPageScrollStateChangedPageScrollState::Settling};
               m_swiperNodeDelegate->onPageScrollStateChanged(event);
        }
        else if (event->kind == ArkUI_NodeEventType::NODE_SWIPER_EVENT_ON_ANIMATION_END) {
                LOG(INFO) << "onNodeEvent-->NODE_SWIPER_EVENT_ON_ANIMATION_END: " << event->componentEvent.data[0].i32;
                this->setDuration(100);
                facebook::react::RNCViewPagerEventEmitter::OnPageScroll m_onPageScroll = {
                      static_cast<double>(event->componentEvent.data[0].i32),0};
                m_swiperNodeDelegate->onPageScroll(m_onPageScroll);
                facebook::react::RNCViewPagerEventEmitter::OnPageScrollStateChanged pageScrollStateChange = {
                      facebook::react::RNCViewPagerEventEmitter::OnPageScrollStateChangedPageScrollState::Idle};   
                m_swiperNodeDelegate->onPageScrollStateChanged(pageScrollStateChange);
            
        }
        else if (event->kind == ArkUI_NodeEventType::NODE_SWIPER_EVENT_ON_CHANGE) {
                LOG(INFO) << "onNodeEvent-->NODE_SWIPER_EVENT_ON_CHANGE: " << event->componentEvent.data[0].i32;
                m_swiperNodeDelegate->onPageSelected(event->componentEvent.data[0].i32);
                facebook::react::RNCViewPagerEventEmitter::OnPageScroll m_onPageScroll = {
                      static_cast<double>(event->componentEvent.data[0].i32),0};
                m_swiperNodeDelegate->onPageScroll(m_onPageScroll);
        }
        else if (event->kind == ArkUI_NodeEventType::NODE_SWIPER_EVENT_ON_GESTURE_SWIPE) {
                LOG(INFO) << "onNodeEvent-->NODE_SWIPER_EVENT_GESTURE_SWIPE";
               if(!m_swiperNodeDelegate->getScrollEnabled() || m_swiperNodeDelegate->getNativeLock()) return;
               int offset = 0;
               int finalIndex = event->componentEvent.data[0].i32;
               float currentOffset = event->componentEvent.data[1].f32;
               if (finalIndex == 0 && abs(currentOffset) < 22) {
                  return;
               }                    
               facebook::react::RNCViewPagerEventEmitter::OnPageScrollStateChanged event = {
                    facebook::react::RNCViewPagerEventEmitter::OnPageScrollStateChangedPageScrollState::Dragging};
               m_swiperNodeDelegate->onPageScrollStateChanged(event); 
               int m_LayoutMetricsWidth = m_swiperNodeDelegate->getLayoutMetricsWidth();
               if (currentOffset < 0) {
                   offset = abs(currentOffset / m_LayoutMetricsWidth);
               } else { 
                  finalIndex = (finalIndex - 1) > 0 ? (finalIndex - 1) : 0;
                  offset = 1 - abs(currentOffset / m_LayoutMetricsWidth);
               }
               if (offset > 1) {
                   offset = offset - 1;
                   finalIndex = finalIndex + 1;
               } else if (offset < 0 && finalIndex >= 1) {
                   offset = offset + 1;
                   finalIndex = finalIndex - 1;
               }
               facebook::react::RNCViewPagerEventEmitter::OnPageScroll m_onPageScroll = {
                      static_cast<double>(finalIndex),static_cast<double>(offset)};                    
               m_swiperNodeDelegate->onPageScroll(m_onPageScroll);
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

   SwiperNode &SwiperNode::setItemSpace(int const &pageMargin) { 
        ArkUI_NumberValue indexValue[] = {{.i32 = pageMargin}};
        ArkUI_AttributeItem indexItem = {indexValue, sizeof(indexValue) / sizeof(ArkUI_NumberValue)};
        maybeThrow(NativeNodeApi::getInstance()->setAttribute(m_nodeHandle, NODE_SWIPER_ITEM_SPACE,&indexItem));
        return *this;
    }

   SwiperNode &SwiperNode::setCachedCount(int const &offscreenPageLimit) { 
        ArkUI_NumberValue indexValue[] = {{.i32 = offscreenPageLimit}};
        ArkUI_AttributeItem indexItem = {indexValue, sizeof(indexValue) / sizeof(ArkUI_NumberValue)};
        maybeThrow(NativeNodeApi::getInstance()->setAttribute(m_nodeHandle, NODE_SWIPER_DISPLAY_COUNT,&indexItem));
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

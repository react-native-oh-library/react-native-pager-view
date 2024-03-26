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
    
        bool m_scrollEnabled;
    
        bool m_nativeLock;

        std::string m_keyboardDismissMode;

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
    
        std::vector<TouchTarget::Shared> getTouchTargetChildren() const override;
    
        bool getScrollEnabled() override;
    
        bool getNativeLock() override;
    
        int getLayoutMetricsWidth() override;
    
        void setKeyboardDismiss() override;
    
        void setNativeResponderBlocked(bool blocked) override;
    };
} // namespace rnoh
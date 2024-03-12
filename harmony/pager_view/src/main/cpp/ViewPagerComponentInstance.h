#pragma once
#include "RNOH/CppComponentInstance.h"
#include "SwiperNode.h"
#include "EventEmitters.h"
#include <folly/dynamic.h>
#include<math.h>
namespace rnoh {
    class ViewPagerComponentInstance : public CppComponentInstance, public SwiperNodeDelegate {
    private:
        SwiperNode m_swiperNode;

        std::shared_ptr<const facebook::react::RNCViewPagerEventEmitter> m_viewPagerEventEmitter;
    
        double m_PageIndex;
    
        bool m_scrollEnabled;
    
        bool m_nativeLock;
    
        int m_layoutMetricsWidth;

    public:
        ViewPagerComponentInstance(Context context, facebook::react::Tag tag);

        void insertChild(ComponentInstance::Shared childComponentInstance, std::size_t index) override;

        void removeChild(ComponentInstance::Shared childComponentInstance) override;

        SwiperNode &getLocalRootArkUINode() override;

        void setProps(facebook::react::Props::Shared props) override;

        void setEventEmitter(facebook::react::SharedEventEmitter eventEmitter) override;

        void onPageSelected(double pageIndex) override;
    
        void onPageScroll(facebook::react::RNCViewPagerEventEmitter::OnPageScroll pageScroll) override;
    
        void onPageScrollStateChanged(facebook::react::RNCViewPagerEventEmitter::OnPageScrollStateChanged value) override;

        void handleCommand(std::string const &commandName, folly::dynamic const &args) override;
    
        bool getScrollEnabled() override;
    
        bool getNativeLock() override;
    
        void setLayout(facebook::react::LayoutMetrics layoutMetrics) override;
    
        int getLayoutMetricsWidth() override;
    };
} // namespace rnoh
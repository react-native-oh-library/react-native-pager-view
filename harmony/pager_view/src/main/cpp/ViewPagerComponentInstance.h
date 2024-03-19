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
    
        double m_pageIndex;
    
        bool m_scrollEnabled;
    
        bool m_nativeLock;
    
    public:
        ViewPagerComponentInstance(Context context);

        void insertChild(ComponentInstance::Shared childComponentInstance, std::size_t index) override;

        void removeChild(ComponentInstance::Shared childComponentInstance) override;

        SwiperNode &getLocalRootArkUINode() override;

        void setProps(facebook::react::Props::Shared props) override;

        void setEventEmitter(facebook::react::SharedEventEmitter eventEmitter) override;

        void onPageSelected(double pageIndex) override;
    
        void onPageScroll(facebook::react::RNCViewPagerEventEmitter::OnPageScroll pageScroll) override;
    
        void onPageScrollStateChanged(facebook::react::RNCViewPagerEventEmitter::OnPageScrollStateChanged value) override;

        void handleCommand(std::string const &commandName, folly::dynamic const &args) override;
    
        std::vector<TouchTarget::Shared> getTouchTargetChildren() const override;
    
        bool getScrollEnabled() override;
    
        bool getNativeLock() override;
    
        int getLayoutMetricsWidth() override;
    };
} // namespace rnoh
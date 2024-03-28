#include "ViewPagerComponentInstance.h"
#include "Props.h"

namespace rnoh {

    ViewPagerComponentInstance::ViewPagerComponentInstance(Context context)
        : CppComponentInstance(std::move(context)) {
        m_swiperNode.setSwiperNodeDelegate(this);
    }

    void ViewPagerComponentInstance::insertChild(ComponentInstance::Shared childComponentInstance, std::size_t index) {
        CppComponentInstance::insertChild(childComponentInstance, index);
        m_swiperNode.insertChild(childComponentInstance->getLocalRootArkUINode(), index);
    }

    SwiperNode &ViewPagerComponentInstance::getLocalRootArkUINode() { return m_swiperNode; }

    void ViewPagerComponentInstance::removeChild(ComponentInstance::Shared childComponentInstance) {
        CppComponentInstance::removeChild(childComponentInstance);
        m_swiperNode.removeChild(childComponentInstance->getLocalRootArkUINode());
    };

    void ViewPagerComponentInstance::setProps(facebook::react::Props::Shared props) {
        CppComponentInstance::setProps(props);
        if (auto p = std::dynamic_pointer_cast<const facebook::react::RNCViewPagerProps>(props)) {
            this->m_scrollEnabled = p->scrollEnabled;
            this->m_pageIndex = p->initialPage;
            LOG(INFO) << "ViewPagerComponentInstance::setProps: " << p->initialPage;
            this->getLocalRootArkUINode().setIndex(p->initialPage);
            this->getLocalRootArkUINode().setVertical(p->orientation);
            this->getLocalRootArkUINode().setDirection(p->layoutDirection);
            this->getLocalRootArkUINode().setItemSpace(p->pageMargin);
            this->getLocalRootArkUINode().setDisableSwipe(!p->scrollEnabled);
            this->getLocalRootArkUINode().setLoop(false);
            this->getLocalRootArkUINode().setIndicator(false);
            this->getLocalRootArkUINode().setDuration(100);
            if(p->overdrag){
               this->getLocalRootArkUINode().setEffectMode("SPRING");
            }
            else{
               if(p->overScrollMode == "never"){
                   this->getLocalRootArkUINode().setEffectMode("NONE");
                }
               else{
                   this->getLocalRootArkUINode().setEffectMode("FADE");                 
               }
            }
        }
    }

    void ViewPagerComponentInstance::setEventEmitter(facebook::react::SharedEventEmitter eventEmitter) {
        ComponentInstance::setEventEmitter(eventEmitter);
        auto viewPagerEventEmitter =
            std::dynamic_pointer_cast<const facebook::react::RNCViewPagerEventEmitter>(eventEmitter);
        if (viewPagerEventEmitter == nullptr) {
            return;
        }
        m_viewPagerEventEmitter = viewPagerEventEmitter;
    }

    void ViewPagerComponentInstance::onPageSelected(double pageIndex) {
        this->m_pageIndex = pageIndex;
        LOG(INFO) << "ViewPagerComponentInstance::onPageSelected:" << pageIndex ;
        facebook::react::RNCViewPagerEventEmitter::OnPageSelected event = {pageIndex};
        m_viewPagerEventEmitter->onPageSelected(event); 
    }

    void ViewPagerComponentInstance::onPageScroll(facebook::react::RNCViewPagerEventEmitter::OnPageScroll pageScroll) {
        LOG(INFO) << "ViewPagerComponentInstance::onPageScroll" ;
        m_viewPagerEventEmitter->onPageScroll(pageScroll);
    }

    void ViewPagerComponentInstance::onPageScrollStateChanged(facebook::react::RNCViewPagerEventEmitter::OnPageScrollStateChanged value) {
        LOG(INFO) << "ViewPagerComponentInstance::onPageScrollStateChanged" ;
        m_viewPagerEventEmitter->onPageScrollStateChanged(value);
    }

    void ViewPagerComponentInstance::handleCommand(std::string const &commandName, folly::dynamic const &args) {
        LOG(INFO) << "handleCommand-->commandName: " << commandName;
        if (commandName == "setScrollEnabledImperatively" && args.isArray() && args.size() == 1) {
            LOG(INFO) << "handleCommand-->setScrollEnabledImperatively: " << args[0];
            this->m_scrollEnabled=args[0].asBool();
            this->getLocalRootArkUINode().setDisableSwipe(!args[0].asBool());
            if(!args[0].asBool()){
               facebook::react::RNCViewPagerEventEmitter::OnPageScrollStateChanged event = {
                      facebook::react::RNCViewPagerEventEmitter::OnPageScrollStateChangedPageScrollState::Idle};
               m_viewPagerEventEmitter->onPageScrollStateChanged(event);
            }
        }
        else if (commandName == "setPage" && args.isArray() && args.size() == 1){
            LOG(INFO) << "handleCommand-->setPage: " << args[0];
            this->getLocalRootArkUINode().setIndex(args[0].asInt());
        }
        else if (commandName == "setPageWithoutAnimation" && args.isArray() && args.size() == 1){
            LOG(INFO) << "handleCommand-->setPageWithoutAnimation: " << args[0];
            this->getLocalRootArkUINode().setIndex(args[0].asInt());
            this->getLocalRootArkUINode().setDuration(0);
        }
    }

    bool ViewPagerComponentInstance::getScrollEnabled() {
       return this->m_scrollEnabled;
    }

    bool ViewPagerComponentInstance::getNativeLock() {
       return this->m_nativeLock;
    }

    std::vector<TouchTarget::Shared> ViewPagerComponentInstance::getTouchTargetChildren() const {
        auto children = getChildren();
        std::vector<TouchTarget::Shared> result(1);
        result.push_back(children.at(m_pageIndex));
        return result;
    }

    int ViewPagerComponentInstance::getLayoutMetricsWidth(){
       return this->m_layoutMetrics.frame.size.width;
    }

    void ViewPagerComponentInstance::setNativeResponderBlocked(bool blocked) {
        LOG(INFO) << "ViewPagerComponentInstance::setNativeResponderBlocked:" << blocked;
        if (blocked) {
            this->m_nativeLock = true;
            this->getLocalRootArkUINode().setDisableSwipe(true);
            facebook::react::RNCViewPagerEventEmitter::OnPageScrollStateChanged event = {
                facebook::react::RNCViewPagerEventEmitter::OnPageScrollStateChangedPageScrollState::Idle};
            m_viewPagerEventEmitter->onPageScrollStateChanged(event);
        } else {
            this->m_nativeLock = false;
            this->getLocalRootArkUINode().setDisableSwipe(false);
        }
    }

} // namespace rnoh/

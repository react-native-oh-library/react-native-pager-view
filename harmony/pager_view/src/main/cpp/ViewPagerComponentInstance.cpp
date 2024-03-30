#include "ViewPagerComponentInstance.h"
#include "Props.h"

namespace rnoh {

    ViewPagerComponentInstance::ViewPagerComponentInstance(Context context)
        : CppComponentInstance(std::move(context)) {
        m_swiperNode.setSwiperNodeDelegate(this);
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
         this->m_scrollEnabled = props->scrollEnabled;
         this->m_pageIndex = props->initialPage;
         this->m_keyboardDismissMode = props->keyboardDismissMode;
         LOG(INFO) << "ViewPagerComponentInstance::setProps: " << props->initialPage;
         this->getLocalRootArkUINode().setIndex(props->initialPage);
         this->getLocalRootArkUINode().setVertical(props->orientation);
         this->getLocalRootArkUINode().setDirection(props->layoutDirection);
         this->getLocalRootArkUINode().setItemSpace(props->pageMargin);
         this->getLocalRootArkUINode().setDisableSwipe(!props->scrollEnabled);
         this->getLocalRootArkUINode().setLoop(false);
         this->getLocalRootArkUINode().setIndicator(false);
         this->getLocalRootArkUINode().setDuration(100);
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
        LOG(INFO) << "ViewPagerComponentInstance::onPageSelected:" << pageIndex ;
        facebook::react::RNCViewPagerEventEmitter::OnPageSelected event = {pageIndex};
        m_eventEmitter->onPageSelected(event); 
    }

    void ViewPagerComponentInstance::onPageScroll(facebook::react::RNCViewPagerEventEmitter::OnPageScroll pageScroll) {
        LOG(INFO) << "ViewPagerComponentInstance::onPageScroll" ;
        m_eventEmitter->onPageScroll(pageScroll);
    }

    void ViewPagerComponentInstance::onPageScrollStateChanged(facebook::react::RNCViewPagerEventEmitter::OnPageScrollStateChanged value) {
        LOG(INFO) << "ViewPagerComponentInstance::onPageScrollStateChanged" ;
        m_eventEmitter->onPageScrollStateChanged(value);
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
               m_eventEmitter->onPageScrollStateChanged(event);
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

    void ViewPagerComponentInstance::setKeyboardDismiss() {
        LOG(INFO) << "ViewPagerComponentInstance::setKeyboardDismiss: " << this->m_keyboardDismissMode ;
        if (this->m_keyboardDismissMode != "on-drag") return;
        auto rnInstancePtr =this->m_deps->rnInstance.lock();
        if (rnInstancePtr != nullptr) {
            auto turboModule = rnInstancePtr->getTurboModule("RNCViewPagerContext");
            auto arkTsTurboModule = std::dynamic_pointer_cast<rnoh::ArkTSTurboModule>(turboModule);
            arkTsTurboModule->callSync("keyboardDismiss", {});
        }
    }

    void ViewPagerComponentInstance::setNativeResponderBlocked(bool blocked) {
        LOG(INFO) << "ViewPagerComponentInstance::setNativeResponderBlocked:" << blocked;
        if(blocked){
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

} // namespace rnoh/

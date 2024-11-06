
/**
 * This code was generated by [react-native-codegen](https://www.npmjs.com/package/react-native-codegen).
 *
 * Do not edit this file as changes may cause incorrect behavior and will be lost
 * once the code is regenerated.
 *
 * @generated by codegen project: GeneratePropsH.js
 */
#pragma once

#include <react/renderer/components/view/ViewProps.h>
#include <react/renderer/core/PropsParserContext.h>

namespace facebook {
namespace react {

enum class RNCViewPagerLayoutDirection { Ltr, Rtl };

static inline void fromRawValue(const PropsParserContext& context, const RawValue &value, RNCViewPagerLayoutDirection &result) {
  auto string = (std::string)value;
  if (string == "ltr") { result = RNCViewPagerLayoutDirection::Ltr; return; }
  if (string == "rtl") { result = RNCViewPagerLayoutDirection::Rtl; return; }
  abort();
}

static inline std::string toString(const RNCViewPagerLayoutDirection &value) {
  switch (value) {
    case RNCViewPagerLayoutDirection::Ltr: return "ltr";
    case RNCViewPagerLayoutDirection::Rtl: return "rtl";
  }
}
enum class RNCViewPagerOrientation { Horizontal, Vertical };

static inline void fromRawValue(const PropsParserContext& context, const RawValue &value, RNCViewPagerOrientation &result) {
  auto string = (std::string)value;
  if (string == "horizontal") { result = RNCViewPagerOrientation::Horizontal; return; }
  if (string == "vertical") { result = RNCViewPagerOrientation::Vertical; return; }
  abort();
}

static inline std::string toString(const RNCViewPagerOrientation &value) {
  switch (value) {
    case RNCViewPagerOrientation::Horizontal: return "horizontal";
    case RNCViewPagerOrientation::Vertical: return "vertical";
  }
}
enum class RNCViewPagerOverScrollMode { Auto, Always, Never };

static inline void fromRawValue(const PropsParserContext& context, const RawValue &value, RNCViewPagerOverScrollMode &result) {
  auto string = (std::string)value;
  if (string == "auto") { result = RNCViewPagerOverScrollMode::Auto; return; }
  if (string == "always") { result = RNCViewPagerOverScrollMode::Always; return; }
  if (string == "never") { result = RNCViewPagerOverScrollMode::Never; return; }
  abort();
}

static inline std::string toString(const RNCViewPagerOverScrollMode &value) {
  switch (value) {
    case RNCViewPagerOverScrollMode::Auto: return "auto";
    case RNCViewPagerOverScrollMode::Always: return "always";
    case RNCViewPagerOverScrollMode::Never: return "never";
  }
}
enum class RNCViewPagerKeyboardDismissMode { None, OnDrag };

static inline void fromRawValue(const PropsParserContext& context, const RawValue &value, RNCViewPagerKeyboardDismissMode &result) {
  auto string = (std::string)value;
  if (string == "none") { result = RNCViewPagerKeyboardDismissMode::None; return; }
  if (string == "on-drag") { result = RNCViewPagerKeyboardDismissMode::OnDrag; return; }
  abort();
}

static inline std::string toString(const RNCViewPagerKeyboardDismissMode &value) {
  switch (value) {
    case RNCViewPagerKeyboardDismissMode::None: return "none";
    case RNCViewPagerKeyboardDismissMode::OnDrag: return "on-drag";
  }
}

class RNCViewPagerProps final : public ViewProps {
 public:
  RNCViewPagerProps() = default;
  RNCViewPagerProps(const PropsParserContext& context, const RNCViewPagerProps &sourceProps, const RawProps &rawProps);

#pragma mark - Props

  bool scrollEnabled{true};
  RNCViewPagerLayoutDirection layoutDirection{RNCViewPagerLayoutDirection::Ltr};
  int initialPage{0};
  RNCViewPagerOrientation orientation{RNCViewPagerOrientation::Horizontal};
  int offscreenPageLimit{0};
  int pageMargin{0};
  RNCViewPagerOverScrollMode overScrollMode{RNCViewPagerOverScrollMode::Auto};
  bool overdrag{false};
  RNCViewPagerKeyboardDismissMode keyboardDismissMode{RNCViewPagerKeyboardDismissMode::None};
};

} // namespace react
} // namespace facebook

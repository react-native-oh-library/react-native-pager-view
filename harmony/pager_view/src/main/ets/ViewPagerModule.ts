import { TurboModule } from "rnoh/ts";
import inputMethod from '@ohos.inputMethod';

export class ViewPagerModule extends TurboModule {

  keyboardDismiss(): void {
    let inputMethodController = inputMethod.getInputMethodController();
    inputMethodController.stopInputSession()
  }

}
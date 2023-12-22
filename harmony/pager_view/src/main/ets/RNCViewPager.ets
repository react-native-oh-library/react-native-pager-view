/**
 * MIT License
 *
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
import{
  Descriptor,
  RNOHContext,
  ViewBaseProps,
  RNViewBase,
  ComponentBuilderContext,
  RNComponentFactory,
  rnComponentFactoryBuilder,
  ViewDescriptorWrapper,
  ViewRawProps,
  ViewDescriptorWrapperBase,
  ViewBaseDescriptor,
  TransformMatrix,
  Tag,
  RNOHComponentCommand
} from 'rnoh'
import {ViewModifier} from './ViewModifier';

import inputMethod from '@ohos.inputMethod';
import { RNCViewPagerManager } from './RNCViewPagerManager';
import Logger from './Logger';

export const PAGER_VIEW_TYPE:string = "RNCViewPager"
const TAG: string = "[RNOH] RNCViewPager"

export interface ViewPagerProps extends ViewBaseProps{
  initialPage:number
  layoutDirection:string
  orientation:string
  pageMargin:number
  offscreenPageLimit:number
  scrollEnabled:boolean
  overScrollMode:string
  overdrag:boolean
  keyboardDismissMode:string
}

export interface ViewPagerRawProps extends ViewRawProps{
}

export interface ViewPagerState{
}

export  type ViewPagerDescriptor = Descriptor<"RNCViewPager",ViewPagerProps,ViewPagerState,ViewPagerRawProps>

export  class ViewPagerDescriptorWrapper extends ViewDescriptorWrapperBase<"RNCViewPager",ViewPagerProps,ViewPagerState,ViewPagerRawProps>{

}

@Component
export struct RNCViewPager{
 @State private descriptorWrapper:ViewPagerDescriptorWrapper | undefined = undefined

  ctx!: RNOHContext
  tag: number = 0
  @BuilderParam buildCustomComponent: (ComponentBuilderContext: ComponentBuilderContext) => void
  @State descriptor: ViewPagerDescriptor = {} as ViewPagerDescriptor
  private swiperController: SwiperController = new SwiperController()
  private unregisterDescriptorChangesListener?: () => void = undefined
  private cleanupCommandCallback?: () => void = undefined
  private unregisterComponentManager?: () => void = undefined
  private defaultDuration: number = 100
  @State pageIndex: number = 0
  @State pageScrollEnabled: boolean = true
  @State isNativeLock:boolean = false
  @State AnimationDuration: number = this.defaultDuration
  private inputMethodController = inputMethod.getInputMethodController();
  private onMoving: boolean = false
  private componentManager!: RNCViewPagerManager

  aboutToAppear(){
    this.initComponent()
    this.descriptor = this.ctx.descriptorRegistry.getDescriptor<ViewPagerDescriptor>(this.tag)
    Logger.info(TAG,`props:${JSON.stringify(this.descriptor.props)}`)
    //this.unregisterDescriptorChangesListener = this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag,
    //  (newDescriptor)=>{
    //    this.descriptor = (newDescriptor as ViewPagerDescriptor)
    //  }
    //)
    this.pageIndex = this.descriptor.props.initialPage
    this.pageScrollEnabled = this.descriptor.props.scrollEnabled
    this.cleanupCommandCallback = this.ctx.commandDispatcher.registerCommandCallback(
      this.tag,
      (command,args:(boolean|number)[]) => {
        Logger.info(TAG,`CommandCallback,command: ${command}`)
        if(command === 'setPage'){
          Logger.info(TAG,`setPage,selectedPage: ${args[0]}`)
          this.pageIndex = args[0] as number
        }else if(command === 'setPageWithoutAnimation'){
          Logger.info(TAG,`setPageWithoutAnimation,selectedPage: ${args[0]}`)
          this.AnimationDuration = 0
          this.pageIndex = args[0] as number
        }else if(command === 'setScrollEnabledImperatively'){
          Logger.info(TAG,`setScrollEnabledImperatively,scrollEnabled: ${args[0]}`)
          this.pageScrollEnabled = Boolean(args[0])
        } else if (command === RNOHComponentCommand.BLOCK_NATIVE_RESPONDER) {
          this.isNativeLock = true
        } else if (command === RNOHComponentCommand.UNBLOCK_NATIVE_RESPONDER) {
          this.isNativeLock = false
        }
      });

    //this.componentManager = new RNCViewPagerManager(this.tag,this.ctx,this.pageIndex)
    this.componentManager.setPage(this.pageIndex)
    //this.unregisterComponentManager = this.ctx.componentManagerRegistry.registerComponentManager(this.tag,this.componentManager)
  }

  initComponent() {
     let descriptor = this.ctx.descriptorRegistry.getDescriptor<ViewBaseDescriptor>(this.tag)
     if(!this.componentManager){
       this.componentManager = new RNCViewPagerManager(this.tag,this.ctx,0)
     }
    this.unregisterComponentManager = this.ctx.componentManagerRegistry.registerComponentManager(this.tag,this.componentManager)
    this.setDescriptor(descriptor)
    const parentTag = descriptor.parentTag
    this.componentManager.setParentTag(parentTag)
    this.unregisterDescriptorChangesListener = this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag,
      (newDescriptor)=>{
        this.descriptor = (newDescriptor as ViewPagerDescriptor)
        setDescriptor(newDescriptor)
      }
    )
  }

  private setDescriptor(descriptor:ViewBaseDescriptor){
    this.descriptorWrapper = new ViewPagerDescriptorWrapper(descriptor)
  }

  aboutToDisappear(){
    this.cleanupCommandCallback?.();
    this.unregisterDescriptorChangesListener?.()
    this.unregisterComponentManager?.()
  }

  getEffectMode(){
    if(this.descriptor.props.overdrag){
      return EdgeEffect.Spring
    }else{
      if (this.descriptor.props.overScrollMode == "never") {
        return EdgeEffect.None
      } else {
        return EdgeEffect.Fade
      }
    }
  }

  onChange(index:number){
    this.pageIndex = index;
    this.componentManager.setPage(this.pageIndex)
    Logger.info(TAG,`onChange,${JSON.stringify({
      type:"selected",
      pageIndex:index
    })}`)
    this.ctx.rnInstanceManager.emitComponentEvent(
      this.descriptor.tag,
      PAGER_VIEW_TYPE,
      {
        type:"selected",
        pageIndex:index
      }
    );

    this.ctx.rnInstanceManager.emitComponentEvent(
      this.descriptor.tag,
      PAGER_VIEW_TYPE,
      {
        type:"scroll",
        position:index,
        offset:0
      }
    );
  }

  onAnimationStart(index:number){
    Logger.info(TAG,`onAnimationStart,index number:${index}`)
    if (this.descriptor.props.keyboardDismissMode == "on-drag") {
      this.inputMethodController.stopInput().then((result) => {
        if (result) {
          Logger.info(TAG,'Success to stopInput.(promise)');
        } else {
          Logger.info(TAG,'Failed to stopInput.(promise)');
        }
      }).catch((err:string | Error) => {
        Logger.error(TAG,'stopInput promise err:' + JSON.stringify(err));
      })
    }
    if (!this.onMoving) {
      this.emitTouchEvent("settling");
    }
 
  }

  onAnimationEnd(index:number){
    Logger.info(TAG,`onAnimationEnd,index number:${index}`)
    if (!this.onMoving) {
      this.emitTouchEvent("idle");
    }
    this.AnimationDuration = this.defaultDuration

    this.ctx.rnInstanceManager.emitComponentEvent(
      this.descriptor.tag,
      PAGER_VIEW_TYPE,
      {
        type:"scroll",
        position:index,
        offset:0
      }
    );
    
  }

  onTouchEvent(event:TouchEvent){
    Logger.info(TAG,`onTouch,touch evet type:${event.type}`)
    if (event.type === TouchType.Move) {
      if (!this.onMoving) {
        this.emitTouchEvent("dragging");
      }
      this.onMoving = true;
    }
    if (event.type === TouchType.Up) {
      this.onMoving = false;
    }
  }

  emitTouchEvent(event:string){
    this.ctx.rnInstanceManager.emitComponentEvent(
      this.descriptor.tag,
      PAGER_VIEW_TYPE,
      {
        type:"scroll_state_changed",
        pageScrollState:event
      }
    );
  }

  build(){
 
      Swiper(this.swiperController){
        ForEach(this.descriptor.childrenTags,(tag:Tag) => {
          Stack(){
            //RNComponentFactory({ ctx:this.ctx,tag:tag,buildCustomComponent:this.buildCustomComponent })
            rnComponentFactoryBuilder(this.ctx,tag,this.ctx.rnInstance.getComponentNameFromDescriptorType(this.ctx.descriptorRegistry.getDescriptor(tag)?.type))
          }
          .width("100%")
          .height("100%")
        },(tag:Tag) => tag.toString())
      }
      .attributeModifier(ViewModifier.getInstance().setDescriptor(this.descriptorWrapper))
      .id(this.tag.toString())
      .accessibilityGroup(this.descriptorWrapper?.accessibilityGroup)
      .accessibilityText(this.descriptorWrapper?.accessibilityText)
      .accessibilityLevel(this.descriptorWrapper?.accessibilityLevel)
      .accessibilityDescription(this.descriptorWrapper?.accessibilityDescription)

      .index(this.pageIndex)
      .indicator(false)
      .vertical(this.descriptor.props.orientation == "horizontal" ? false : true)
      .direction(this.descriptor.props.layoutDirection == "ltr" ? Direction.Ltr : Direction.Rtl)
      .itemSpace(this.descriptor.props.pageMargin)
      .cachedCount(this.descriptor.props.offscreenPageLimit)
      .disableSwipe(!this.pageScrollEnabled || this.isNativeLock)
      .effectMode(this.getEffectMode())
      .duration(this.AnimationDuration)
      .loop(false)

      .onChange((index:number)=>this.onChange(index))
      .onAnimationStart((index:number)=>this.onAnimationStart(index))
      .onAnimationEnd((index:number)=>this.onAnimationEnd(index))
      .onTouch( (event:TouchEvent)=>{
        this.onTouchEvent(event)
      })
      .onGestureSwipe((index:number,extraInfo:SwiperAnimationEvent)=>{
        Logger.debug(TAG,`onGestureSwipe,onGestureSwipe:${index},extraInfo:${JSON.stringify(extraInfo)}`)
        //1216修改，规避起始边缘tab异常场景（双指右滑，选项卡异常）,22是测试观察取的经验值
        if (index == 0 && Math.abs(extraInfo.currentOffset) < 22) {
          Logger.debug(TAG,`onGestureSwipe,ignore，limit offset:${index},extraInfo:${JSON.stringify(extraInfo)}`)
          return
        }
         //1216修改，规避起始边缘tab异常场景（双指右滑，选项卡异常）
        if (!this.onMoving) {
          Logger.debug(TAG,`onGestureSwipe,ignore，no in gap:${index},extraInfo:${JSON.stringify(extraInfo)}`)
          return
        }
        // 规避起始边缘tab异常场景（双指右滑，选项卡异常）
        if (index == 0 && extraInfo.currentOffset == 0) {
          return
        }
        this.emitTouchEvent("dragging");

        let offset = 0;
        let finalIndex = index;
        if (extraInfo.currentOffset < 0) {
          offset = Math.abs(extraInfo.currentOffset/this.descriptor.layoutMetrics.frame.size.width)
        } else {
          finalIndex = (index - 1) > 0 ? (index - 1) : 0
          offset = 1 - Math.abs(extraInfo.currentOffset/this.descriptor.layoutMetrics.frame.size.width)
        }

        if (offset > 1) {
          offset = offset - 1
          finalIndex = finalIndex + 1
        }else if(offset < 0 && finalIndex >= 1){
          offset = offset + 1
          finalIndex = finalIndex - 1
        }

        this.ctx.rnInstanceManager.emitComponentEvent(
          this.descriptor.tag,
          PAGER_VIEW_TYPE,
          {
            type:"scroll",
            position:finalIndex,
            offset:offset
          }
        );
      })
  }
  
}
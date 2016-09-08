/*
 * main.cpp
 *
 *  Created on: 20 февр. 2014 г.
 *      Author: g.kruglov
 */

#include "main.h"
//#include "SimpleSensors.h"
//#include "buttons.h"
#include "board.h"
#include "led.h"
//#include "Sequences.h"
//#include "kl_adc.h"

#if 1 // ======================== Variables and defines ========================
App_t App;

// ==== Periphery ====
#if BTN_ENABLED
#define BTN_POLL_PERIOD_MS   27
static void TmrBtnCallback(void *p);

class Btn_t {
private:
    virtual_timer_t Tmr;
    bool IWasPressed = false;
    PinInput_t Pin {BTN_PIN};
public:
    void Init() {
        chVTSet(&Tmr, MS2ST(BTN_POLL_PERIOD_MS), TmrBtnCallback, nullptr);
        Pin.Init();
    }
    void OnTmrTick() {}
    void ITmrCallback() {
        chSysLockFromISR();
        if(Pin.IsHi() and !IWasPressed) {
            IWasPressed = true;
            App.SignalEvtI(EVT_BUTTON);
        }
        else if(!Pin.IsHi() and IWasPressed) IWasPressed = false;
        chVTSetI(&Tmr, MS2ST(BTN_POLL_PERIOD_MS), TmrBtnCallback, nullptr);
        chSysUnlockFromISR();
    }
} Btn;

void TmrBtnCallback(void *p) { Btn.ITmrCallback(); }
#endif

// ==== Timers ====
//static TmrKL_t TmrEverySecond {MS2ST(1000), EVT_EVERY_SECOND, tktPeriodic};
#endif

int main(void) {
    // ==== Init Vcore & clock system ====
    Clk.UpdateFreqValues();

    // === Init OS ===
    halInit();
    chSysInit();
    App.InitThread();

    // ==== Init hardware ====
    Uart.Init(115200, UART_GPIO, UART_TX_PIN, UART_GPIO, UART_RX_PIN);
    Uart.Printf("\r%S %S\r", APP_NAME, BUILD_TIME);
//    Uart.Printf("ID: %X %X %X\r", GetUniqID1(), GetUniqID2(), GetUniqID3());
//    if(Sleep::WasInStandby()) {
//        Uart.Printf("WasStandby\r");
//        Sleep::ClearStandbyFlag();
//    }
    Clk.PrintFreqs();
//    RandomSeed(GetUniqID3());   // Init random algorythm with uniq ID

    // === LED ===
    PinSetupOut(GPIOA, 7, omPushPull);
    PinSetHi(GPIOA, 7);

//    Led.Init();
//    Led.SetupSeqEndEvt(chThdGetSelfX(), EVT_LED_SEQ_END);

//    Btn.Init();
//    Adc.Init();

//    TmrEverySecond.InitAndStart();

    // Main cycle
    App.ITask();
}

__noreturn
void App_t::ITask() {
    while(true) {
        __unused eventmask_t Evt = chEvtWaitAny(ALL_EVENTS);
        if(Evt & EVT_EVERY_SECOND) {

        }

#if 0 // ==== Led sequence end ====
        if(Evt & EVT_LED_SEQ_END) {
        }
#endif

#if BTN_ENABLED
        if(Evt & EVT_BUTTON) {
            if(Mode == modeBinding) Binding.ProcessEvt(bevtBtn);
        }
#endif

#if 0 // ==== Vibro seq end ====
        if(Evt & EVT_VIBRO_END) {
            // Restart vibration (or start new one) if needed
            if(pVibroSeqToPerform != nullptr) Vibro.StartSequence(pVibroSeqToPerform);
        }
#endif

#if UART_RX_ENABLED
        if(Evt & EVT_UART_NEW_CMD) {
            OnCmd((Shell_t*)&Uart);
            Uart.SignalCmdProcessed();
        }
#endif

    } // while true
}

#if UART_RX_ENABLED // ================= Command processing ====================
void App_t::OnCmd(Shell_t *PShell) {
	Cmd_t *PCmd = &PShell->Cmd;
    __attribute__((unused)) int32_t dw32 = 0;  // May be unused in some configurations
    Uart.PrintfNow("%S\r", PCmd->Name);
    // Handle command
    if(PCmd->NameIs("Ping")) {
        PShell->Ack(OK);
    }

//    else if(PCmd->NameIs("SetID")) {
//        if(PCmd->GetNextNumber(&dw32) != OK) { PShell->Ack(CMD_ERROR); return; }
//        uint8_t r = ISetID(dw32);
//        PShell->Ack(r);
//    }

    else PShell->Ack(CMD_UNKNOWN);
}
#endif

/*
 * main.cpp
 *
 *  Created on: 20 февр. 2014 г.
 *      Author: g.kruglov
 */

#include "main.h"
#include "SimpleSensors.h"
#include "buttons.h"
#include "board.h"
#include "led.h"
//#include "Sequences.h"
#include "kl_adc.h"

App_t App;

static TmrKL_t TmrPwrPoll {MS2ST(99), EVT_PWR_POLL, tktPeriodic};
static const PinOutput_t PinPwr {PWR_PIN};

// ==== Digital Potentiometer ====
class DigPot_t {
private:
    int32_t Value = 0;
    const PinOutput_t UD {DPOT_UD_PIN};
    const PinOutput_t CS {DPOT_CS_PIN};
public:
    void Increase() {
        if     (Value < 10) MoveUp(1);
        else if(Value < 20) MoveUp(2);
        else if(Value < 30) MoveUp(3);
        else if(Value < 40) MoveUp(4);
        else if(Value < 50) MoveUp(5);
        else if(Value < 60) MoveUp(6);
    }
    void Decrease() {
        if(Value == 1) return;
        if     (Value < 10) MoveDown(1);
        else if(Value < 20) MoveDown(2);
        else if(Value < 30) MoveDown(3);
        else if(Value < 40) MoveDown(4);
        else if(Value < 50) MoveDown(5);
        else if(Value < 60) MoveDown(6);
    }

    void MoveDown(int32_t Cnt) {
        Value -= Cnt;
        if(Value < 0) Value = 0;
        chSysLock();
        UD.Lo();
        CS.Lo();
        while(Cnt != 0) {
            UD.Lo();
            UD.Hi();
            Cnt--;
        }
        CS.Hi();
        chSysUnlock();
    }
    void MoveUp(int32_t Cnt) {
        Value += Cnt;
        if(Value > 64) Value = 64;
        chSysLock();
        UD.Hi();
        CS.Lo();
        while(Cnt != 0) {
            UD.Lo();
            UD.Hi();
            Cnt--;
        }
        CS.Hi();
        chSysUnlock();
    }
    void Init() {
        UD.Init();
        CS.Init();
        CS.Hi();
        MoveDown(72);
        MoveUp(1);
    }
};

DigPot_t DigPot;

int main(void) {
    // ==== Init Vcore & clock system ====
    Clk.SetupBusDividers(ahbDiv4, apbDiv1);
    Clk.UpdateFreqValues();

    // === Init OS ===
    halInit();
    chSysInit();
    App.InitThread();

    // ==== Init hardware ====
    Uart.Init(115200, UART_GPIO, UART_TX_PIN);//, UART_GPIO, UART_RX_PIN);
    Uart.Printf("\r%S %S\r", APP_NAME, BUILD_TIME);
    Clk.PrintFreqs();

    PinPwr.Init();
    PinPwr.Hi();    // Keep power on

    // === LED ===
    PinSetupOut(GPIOA, 7, omPushPull);
    PinSetHi(GPIOA, 7);

//    Led.Init();
//    Led.SetupSeqEndEvt(chThdGetSelfX(), EVT_LED_SEQ_END);

    PinSensors.Init();
    DigPot.Init();
    Adc.Init();
    Adc.EnableVRef();

    TmrPwrPoll.InitAndStart();

    // Main cycle
    App.ITask();
}

__noreturn
void App_t::ITask() {
    bool AdcFirstConv = true, PwrBtnWasPressed = false, FirstReleaseOccured = false;
    while(true) {
        __unused eventmask_t Evt = chEvtWaitAny(ALL_EVENTS);
        if(Evt & EVT_PWR_POLL) {
            Adc.StartMeasurement();
        }

#if 1 // Buttons
        if(Evt & EVT_BUTTONS) {
            BtnEvtInfo_t EInfo;
            while(BtnGetEvt(&EInfo) == OK) {
                if(EInfo.Type == bePress or EInfo.Type == beRepeat) {
                    if(EInfo.BtnID == btnUp) {
//                        Uart.Printf("Up\r");
                        DigPot.Increase();
                    }
                    else if(EInfo.BtnID == btnDown) {
//                        Uart.Printf("Down\r");
                        DigPot.Decrease();
                    }
                }
            }
        }
#endif

#if ADC_REQUIRED
        if(Evt & EVT_ADC_DONE) {
            if(AdcFirstConv) AdcFirstConv = false;
            else {
                uint32_t VBtnAdc = Adc.GetResult(3);
                uint32_t VRef = Adc.GetResult(ADC_VREFINT_CHNL);
                uint32_t VBtn_mv = Adc.Adc2mV(VBtnAdc, VRef);
    //            Uart.Printf("adc: %u; Vref: %u; VBtn: %u\r", VBtnAdc, VRef, VBtn_mv);
                // Process btn press
                bool PwrBtnIsPressed = (VBtn_mv > 2700);
//                Uart.Printf("Pressed: %u; FR: %u; WP: %u\r", PwrBtnIsPressed, FirstReleaseOccured, PwrBtnWasPressed);
                if(FirstReleaseOccured) {
                    if(PwrBtnIsPressed and !PwrBtnWasPressed) {
                        PwrBtnWasPressed = true;
                    }
                    else if(!PwrBtnIsPressed and PwrBtnWasPressed) {
                        // switch off
                        PinPwr.Lo();
                    }
                }
                if(!PwrBtnIsPressed and !FirstReleaseOccured) {
                    FirstReleaseOccured = true; // Do not react on first press / release
                }
            }
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

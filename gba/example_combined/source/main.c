// GBT Player v4.2.0
//
// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>

#include <gba.h>
#include <maxmod.h>

#include "gbt_player.h"

#include "soundbank.h"
#include "soundbank_bin.h"

extern const uint8_t *template_combined_psg[];

int main(int argc, char *argv[])
{
    irqInit();

    irqSet(IRQ_VBLANK, mmVBlank);
    irqSet(IRQ_TIMER2, gbt_update);

    irqEnable(IRQ_VBLANK | IRQ_TIMER2);

    uint32_t gba_cpu_freq = 16 * 1024 * 1024;

    REG_TM1CNT_L = -1024;
    REG_TM1CNT_H = TIMER_IRQ | TIMER_START; // Prescaler = 1

    REG_TM2CNT_L = -(gba_cpu_freq / 1024) / 64;
    REG_TM2CNT_H = TIMER_COUNT | TIMER_IRQ | TIMER_START;

    // Initialize maxmod with soundbank and 4 channels
    mmInitDefault((mm_addr)soundbank_bin, 4);
    // PSG channels have 1/4th of the range of DMA channels
    mmSetModuleVolume(1024 / 4);

    // Start both songs
    gbt_play(template_combined_psg, -1);
    gbt_loop(1);
    mmStart(MOD_TEMPLATE_COMBINED_DMA, MM_PLAY_LOOP);

    while (1)
    {
        VBlankIntrWait();
        mmFrame();
    }
}

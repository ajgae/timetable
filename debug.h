#pragma once

#include "main.h"

Week debug_week_create_default(void);
Week debug_week_create_true(void);
Day debug_day_create_default(int day_count, int index);
Day debug_day_create_mon(int day_count, int index);
Day debug_day_create_tue(int day_count, int index);
Day debug_day_create_wed(int day_count, int index);
Day debug_day_create_thu(int day_count, int index);
Day debug_day_create_fri(int day_count, int index);

Week debug_week_create_default(void) {
    int day_count = 5;
    Day* days = calloc(day_count, sizeof (Day));
    for (int i = 0; i < day_count; ++i) {
        days[i] = debug_day_create_mon(day_count, i);
    }
    Week week = { .day_count = day_count, .days = days };
    return week;
}

Week debug_week_create_true(void) {
    int day_count = 5;
    Day* days = calloc(day_count, sizeof (Day));

    days[0] = debug_day_create_mon(day_count, 0);
    days[1] = debug_day_create_tue(day_count, 1);
    days[2] = debug_day_create_wed(day_count, 2);
    days[3] = debug_day_create_thu(day_count, 3);
    days[4] = debug_day_create_fri(day_count, 4);

    Week week = { .day_count = day_count, .days = days };
    return week;
}

Day debug_day_create_default(int day_count, int index) {
    Slot* slots = calloc(5, sizeof (Slot));
    slots[0] = slot_create(0*HOUR,              1*QUARTER, "WAKEUP GRAB YOUR BRUSH AND PUT ON A LITTLE MAKEUP");
    slots[1] = slot_create(6*HOUR + 1*QUARTER,  4*QUARTER, "BREAKFAST");
    slots[2] = slot_create(12*HOUR + 2*QUARTER, 4*QUARTER, "LUNCH");
    slots[3] = slot_create(18*HOUR,             1*QUARTER, "SLEEP");
    slots[4] = slot_create(23*HOUR + 0*QUARTER, 1*QUARTER, "NIGHT");
    Day day = day_create(5, slots, DAY_VIRT_HEIGHT, day_count, index);
    return day;
}

Day debug_day_create_mon(int day_count, int index) {
    int slot_count = 2;
    Slot* slots = calloc(slot_count, sizeof (Slot));

    slots[0] = slot_create(10*HOUR + 1*QUARTER, 1*HOUR + 3*QUARTER, "ENG-EX-EngComp");
    slots[1] = slot_create(12*HOUR + 2*QUARTER, 1*HOUR + 2*QUARTER, "ENG-EX-EngComp");

    Day day = day_create(slot_count, slots, DAY_VIRT_HEIGHT, day_count, index);
    return day;
}

Day debug_day_create_tue(int day_count, int index) {
    int slot_count = 2;
    Slot* slots = calloc(slot_count, sizeof (Slot));

    slots[0] = slot_create(15*HOUR + 1*QUARTER, 0*HOUR + 3*QUARTER, "ENG-CC-IELL");
    slots[1] = slot_create(16*HOUR + 1*QUARTER, 0*HOUR + 3*QUARTER, "ENG-TP-IELL");

    Day day = day_create(slot_count, slots, DAY_VIRT_HEIGHT, day_count, index);
    return day;
}

Day debug_day_create_wed(int day_count, int index) {
    int slot_count = 5;
    Slot* slots = calloc(slot_count, sizeof (Slot));

    slots[0] = slot_create(8*HOUR + 2*QUARTER, 1*HOUR + 2*QUARTER, "INF-CT-ProgInt1");
    slots[1] = slot_create(10*HOUR + 1*QUARTER, 1*HOUR + 3*QUARTER, "INF-TP-ProgInt1");
    slots[2] = slot_create(14*HOUR + 1*QUARTER, 1*HOUR + 3*QUARTER, "INF-CT-Database");
    slots[3] = slot_create(16*HOUR + 1*QUARTER, 0*HOUR + 3*QUARTER, "ENG-TP-PEP");
    slots[4] = slot_create(17*HOUR + 0*QUARTER, 0*HOUR + 3*QUARTER, "ENG-TP-PEP");

    Day day = day_create(slot_count, slots, DAY_VIRT_HEIGHT, day_count, index);
    return day;
}

Day debug_day_create_thu(int day_count, int index) {
    int slot_count = 2;
    Slot* slots = calloc(slot_count, sizeof (Slot));

    slots[0] = slot_create(12*HOUR + 2*QUARTER, 1*HOUR + 2*QUARTER, "PHI-CC-PhiAnt");
    slots[1] = slot_create(14*HOUR + 1*QUARTER, 1*HOUR + 3*QUARTER, "PHI-TP-PhiAnt");

    Day day = day_create(slot_count, slots, DAY_VIRT_HEIGHT, day_count, index);
    return day;
}

Day debug_day_create_fri(int day_count, int index) {
    int slot_count = 3;
    Slot* slots = calloc(slot_count, sizeof (Slot));

    slots[0] = slot_create(8*HOUR + 2*QUARTER, 1*HOUR + 2*QUARTER, "PHI-CC-PhiGen&Sys");
    slots[1] = slot_create(10*HOUR + 1*QUARTER, 1*HOUR + 3*QUARTER, "INF-CT-Ana&Con");
    slots[2] = slot_create(14*HOUR + 1*QUARTER, 1*HOUR + 3*QUARTER, "PHI-TP-PhiGen&Sys");

    Day day = day_create(slot_count, slots, DAY_VIRT_HEIGHT, day_count, index);
    return day;
}

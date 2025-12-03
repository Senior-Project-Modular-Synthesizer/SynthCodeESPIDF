#pragma once

typedef enum {
    EMPTY,
    SLIDER, // Input int
    BUTTON, // Input bool
    ARC1,   // Input int
    ARC2,   // Input int
    NUMBER, // Output int
    LIGHT   // Output bool
} UITypes;

typedef struct {
    UITypes type;
    char name[16];
    short min;
    short max;
    short start;
    const void* data;
} UIElement;

static const UIElement EMPTY_ELEMENT = {EMPTY, "", 0, 0, 0, NULL};
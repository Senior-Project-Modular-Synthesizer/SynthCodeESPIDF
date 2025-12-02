enum UITypes {
    SLIDER, // Input int
    BUTTON, // Input bool
    POT1, // Input int
    POT2, // Input int
    NUMBER, // Output float
    LIGHT // Output bool
};

typedef struct {
    UITypes element;
    const char[] name;
    short min;
    short max;
    short start;
} UIElement;
enum UITypes {
    EMPTY,
    SLIDER, // Input int
    BUTTON, // Input bool
    POT1, // Input int
    POT2, // Input int
    NUMBER, // Output float
    LIGHT // Output bool
};

typedef struct {
    UITypes type;
    char name[16];
    short min;
    short max;
    short start;
    void* data;
} UIElement;

static const UIElement EMPTY_ELEMENT = {UITypes.EMPTY, "", 0, 0, 0, NULL};
#include <Processor.hpp>

class LeakyFilter : public Processor {
    private:
        float leakFactor;
        QuadSample lastOutput;

    public:
        LeakyFilter() : leakFactor(0.5f), lastOutput{} {}

        void process(QuadInputBuffer& input, QuadOutputBuffer& output) override {
            const int blockSize = input.size();
            for (int i = 0; i < blockSize; i++) {
                QuadSample inSample = input.nextSample();
                QuadSample outSample;
                for (int ch = 0; ch < 4; ch++) {
                    outSample[ch] = inSample[ch] + leakFactor * (lastOutput[ch] - inSample[ch]);
                    lastOutput[ch] = outSample[ch];
                }
                output.pushSample(outSample);
            }
        }

        int blockSize() const override {
            return 1; // Process one sample at a time
        }

        std::variant<std::map<std::string, std::pair<UIElement, void*>>, CustomUI> getUIType() const override {
            return std::map<std::string, std::pair<UIElement, void*>>{
                {"Leak Factor", {UIElement::SLIDER, (void*)&leakFactor}}
            };
        }
};
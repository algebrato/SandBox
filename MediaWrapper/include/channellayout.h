#ifndef CHANNELLAYOUT_H
#define CHANNELLAYOUT_H

#include <string>
#include <string_view>
#include <bitset>

extern "C" {
#include <libavutil/channel_layout.h>
}

namespace MediaWrapper::AV {

std::string channel_name(AVChannel channel);
std::string channel_description(AVChannel channel);
AVChannel   channel_from_string(const std::string &name);
AVChannel   channel_from_string(const char *name);

class ChannelLayout;

    class ChannelLayoutView
    {
    public:
        ChannelLayoutView() noexcept;
        // implicit by design
        ChannelLayoutView(const AVChannelLayout &layout) noexcept;

        template<typename Callable>
        static void iterate(Callable callable) {
            void *iter = nullptr;
            const AVChannelLayout *playout;
            while ((playout = av_channel_layout_standard(&iter))) {
                if (callable(ChannelLayoutView(*playout)))
                    break;
            }
        }

        static void dumpLayouts();

        AVChannelLayout       *raw();
        const AVChannelLayout *raw() const;

        int         channels() const noexcept;
        uint64_t    layout() const noexcept;
        uint64_t    subset(uint64_t mask) const noexcept;
        bool        isValid() const noexcept;
        std::string describe() const;
        void        describe(std::string &desc) const;

        int index(AVChannel channel) const;
        int index(const char *name) const;

        AVChannel channel(unsigned int index) const;
        AVChannel channel(const char *name) const;

        bool operator==(const ChannelLayoutView &other) const noexcept;

        ChannelLayout clone() const;

    protected:
        AVChannelLayout m_layout{};
    };



    class ChannelLayout : public ChannelLayoutView
    {
    public:
        ChannelLayout() = default;
        //explicit ChannelLayout(ChannelLayoutView &&view);
        explicit ChannelLayout(const ChannelLayoutView &view);

        using ChannelLayoutView::ChannelLayoutView;

        ChannelLayout(const ChannelLayout& other);
        ChannelLayout& operator=(const ChannelLayout& other);

        ChannelLayout(ChannelLayout&& other);
        ChannelLayout& operator=(ChannelLayout&& other);

        ~ChannelLayout();

        //explicit ChannelLayout(uint64_t mask);
        explicit ChannelLayout(std::bitset<64> mask);
        explicit ChannelLayout(const char *str);
        explicit ChannelLayout(int channels);

        // Stop control AVChannelLayout
        void release();

    private:
        void swap(ChannelLayout &other);

    };


} // namespace av

#endif // CHANNELLAYOUT_H
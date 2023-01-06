#include "OverlayManager.h"

#include "Core/CommonCore/Assertion.h"

#include <algorithm>

namespace EngineCore
{
    namespace GUI
    {
        OverlayManager::OverlayManager()
            : mOverlays(),
              mCurrentOpenedOverlay()
        {
        }

        void OverlayManager::RegisterOverlay(std::shared_ptr<IUiOverlay> overlay)
        {
            const auto &foundOverlay = FindOverlay(overlay->GetOverlayName());
            assert(!foundOverlay);
            mOverlays.emplace_back(overlay);
        }

        void OverlayManager::UnregisterOverlay(std::shared_ptr<IUiOverlay> overlay)
        {
            const auto &foundOverlay = FindOverlay(overlay->GetOverlayName());
            assert(foundOverlay);
            auto remove_it = std::remove_if(mOverlays.begin(), mOverlays.end(), [&](const auto &myOverlay)
                                            { return overlay->GetOverlayName() == myOverlay->GetOverlayName(); });
            mOverlays.erase(remove_it);
        }

        std::shared_ptr<IUiOverlay> OverlayManager::GetOverlayByName(const std::string &name)
        {
            return FindOverlay(name);
        }

        void OverlayManager::OpenOverlay(const std::string &overlayName)
        {
            const auto &foundOverlay = FindOverlay(overlayName);
            assert(foundOverlay);
            if (mCurrentOpenedOverlay)
            {
                mCurrentOpenedOverlay->CloseOverlay();
            }
            mCurrentOpenedOverlay = foundOverlay;
            mCurrentOpenedOverlay->OpenOverlay();
        }

        void OverlayManager::CloseCurrentOverlay()
        {
            if (mCurrentOpenedOverlay)
            {
                mCurrentOpenedOverlay->CloseOverlay();
                mCurrentOpenedOverlay = nullptr;
            }
        }

        bool OverlayManager::CurrentOverlayOpened() const
        {
            return mCurrentOpenedOverlay != nullptr;
        }

        std::string OverlayManager::GetCurrentOpenedOverlayName() const
        {
            return mCurrentOpenedOverlay ? mCurrentOpenedOverlay->GetOverlayName() : "";
        }

        void OverlayManager::Tick(const float deltaTime)
        {
            for (const auto &overlay : mOverlays)
            {
                overlay->Tick(deltaTime);
            }
        }

        void OverlayManager::UnpausableTick(const float deltaTime)
        {
            for (const auto &overlay : mOverlays)
            {
                overlay->UnpausableTick(deltaTime);
            }
        }

        void OverlayManager::Initialize()
        {
            for (const auto &overlay : mOverlays)
            {
                overlay->Initialize();
            }
        }

        std::shared_ptr<IUiOverlay> OverlayManager::FindOverlay(const std::string &overlayName) const
        {
            const auto findIt = std::find_if(mOverlays.cbegin(), mOverlays.cend(), [&](const auto &overlay)
                                             { return overlay->GetOverlayName() == overlayName; });
            return findIt != mOverlays.end() ? *findIt : nullptr;
        }
    }
}
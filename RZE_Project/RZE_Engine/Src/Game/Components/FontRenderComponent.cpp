#include <StdAfx.h>

#include <Game/Components/FontRenderComponent.h>

FontRenderComponent::FontRenderComponent()
{
    SetName("FontRenderComponent");
	RegisterComponentID<FontRenderComponent>();
}

FontRenderComponent::FontRenderComponent(const std::string& name)
{
    SetName(name);
	RegisterComponentID<FontRenderComponent>();
}

FontRenderComponent::~FontRenderComponent()
{
    RZE_Engine::Get()->GetResourceHandler().ReleaseResource(mFontResource);
}

void FontRenderComponent::SetFont(const ResourceHandle& fontResource)
{
    mFontResource = fontResource;
}

void FontRenderComponent::SetText(const std::string& text)
{
    mText = text;
}

ResourceHandle FontRenderComponent::GetFont()
{
    return mFontResource;
}

const std::string& FontRenderComponent::GetText()
{
    return mText;
}

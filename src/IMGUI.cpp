/*=============================================================================
Blobby Volley 2
Copyright (C) 2006 Jonathan Sieber (jonathan_sieber@yahoo.de)
Copyright (C) 2006 Daniel Knobe (daniel-knobe@web.de)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
=============================================================================*/

/* header include */
#include "IMGUI.h"

/* includes */
#include <queue>
#include <cassert>

#include <SDL.h>

/* implementation */

enum ObjectType
{
	IMAGE,
	OVERLAY,
	TEXT,
	BUTTON, // Text button with a visible controller focus indicator.
	SCROLLBAR,
	ACTIVESCROLLBAR,
	EDITBOX,
	ACTIVEEDITBOX,
	SELECTBOX,
	ACTIVESELECTBOX,
	BLOB,
	CHAT,
	ACTIVECHAT
};

struct QueueObject
{
	ObjectType type;
	int id;
	Vector2 pos1;
	Vector2 pos2;
	Color col;
	float alpha;
	std::string text;
	std::vector<std::string> entries;
	int selected;
	int length;
	unsigned int flags;
};

IMGUI::IMGUI(InputManager* inputMgr) : mInputManager(inputMgr)
{
	assert(mInputManager);
	mQueue.reset(new RenderQueue);
    mDrawCursor = false;
	mActiveButton = -1;
	mHeldWidget = 0;
	mLastKeyAction = KeyAction::NONE;
	mLastWidget = 0;
	mButtonReset = false;
	mInactive = false;
	mIdCounter = 0;
}

IMGUI::~IMGUI() = default;

void IMGUI::begin()
{
	mUsingCursor = false;
	mButtonReset = false;

	while (!mQueue->empty())
		mQueue->pop();


	mLastKeyAction = KeyAction::NONE;

	if (mInputManager->up())
		mLastKeyAction = KeyAction::UP;

	if (mInputManager->down())
		mLastKeyAction = KeyAction::DOWN;

	if (mInputManager->left())
		mLastKeyAction = KeyAction::LEFT;

	if (mInputManager->right())
		mLastKeyAction = KeyAction::RIGHT;

	if (mInputManager->select())
		mLastKeyAction = KeyAction::SELECT;

	if (mInputManager->exit())
		mLastKeyAction = KeyAction::BACK;

#ifdef BLOBBY_MODERN_UI
    if (!mPreviousMenuItems.empty() && mInputManager->menuBack())
        mLastKeyAction = KeyAction::BACK;
    navigateMenu();
    mMenuItems.clear();
    mPointerClaimed = false;
#endif
	mIdCounter = 0;
}

#ifdef BLOBBY_MODERN_UI
void IMGUI::navigateMenu()
{
    const MenuRect* active = nullptr;
    for (const auto& item : mPreviousMenuItems)
        if (item.id == mActiveButton) active = &item;
    if (!active) { mActiveButton = -1; mEditing = false; return; }
    if (mEditing)
    {
        if (mLastKeyAction == KeyAction::BACK || mLastKeyAction == KeyAction::SELECT)
        {
            mEditing = false;
            SDL_StopTextInput();
            mLastKeyAction = KeyAction::NONE;
        }
        else if (active->kind == SELECTBOX || active->kind == CHAT)
        {
            // Legacy list code changes rows with Left/Right; adapt only while
            // inside a list. Focus navigation outside lists remains spatial.
            if (mLastKeyAction == KeyAction::UP) mLastKeyAction = KeyAction::LEFT;
            else if (mLastKeyAction == KeyAction::DOWN) mLastKeyAction = KeyAction::RIGHT;
            else if (mLastKeyAction == KeyAction::LEFT || mLastKeyAction == KeyAction::RIGHT)
                mLastKeyAction = KeyAction::NONE;
        }
        else if (mLastKeyAction == KeyAction::UP || mLastKeyAction == KeyAction::DOWN)
            mLastKeyAction = KeyAction::NONE;
        return;
    }
    if (mLastKeyAction == KeyAction::SELECT && active->kind != BUTTON)
    {
        mEditing = true;
        if (active->kind == EDITBOX) SDL_StartTextInput();
        mLastKeyAction = KeyAction::NONE;
        return;
    }
    int dx = 0, dy = 0;
    if (mLastKeyAction == KeyAction::LEFT) dx = -1;
    if (mLastKeyAction == KeyAction::RIGHT) dx = 1;
    if (mLastKeyAction == KeyAction::UP) dy = -1;
    if (mLastKeyAction == KeyAction::DOWN) dy = 1;
    if (dx || dy)
    {
        mActiveButton = spatialMenuNext(mPreviousMenuItems, mActiveButton, dx, dy);
        mLastKeyAction = KeyAction::NONE;
    }
}

void IMGUI::trackMenuItem(int id, int kind, const Vector2& a, const Vector2& b)
{
    if (mInactive) return;
    mMenuItems.push_back({id, kind, a.x, a.y, b.x, b.y});
    if (mActiveButton == -1) mActiveButton = id;
    // Initial selection follows visual reading order, not function-call order.
    if (mPreviousMenuItems.empty())
        for (const auto& item : mMenuItems)
            if (item.id == mActiveButton && (a.y < item.y1 || (a.y == item.y1 && a.x < item.x1)))
            { mActiveButton = id; break; }
}
#endif

#ifdef BLOBBY_MODERN_UI
namespace {
void drawFocusFrame(RenderManager& renderer, const Vector2& a, const Vector2& b)
{
    const Color gold(255, 215, 40);
    renderer.drawOverlay(1.0f, a, Vector2(b.x, a.y + 3), gold);
    renderer.drawOverlay(1.0f, Vector2(a.x, b.y - 3), b, gold);
    renderer.drawOverlay(1.0f, a, Vector2(a.x + 3, b.y), gold);
    renderer.drawOverlay(1.0f, Vector2(b.x - 3, a.y), b, gold);
}
}
#endif

void IMGUI::end(RenderManager& renderer)
{
#ifdef BLOBBY_MODERN_UI
    bool found = false;
    for (const auto& item : mMenuItems) if (item.id == mActiveButton) found = true;
    if (!found && !mMenuItems.empty()) { mActiveButton = mMenuItems.front().id; mEditing = false; }
    mPreviousMenuItems = mMenuItems;
#endif
	int FontSize;
	while (!mQueue->empty())
	{
		QueueObject& obj = mQueue->front();
		switch (obj.type)
		{
			case IMAGE:
				renderer.drawImage( obj.text, obj.pos1, obj.pos2);
				break;

			case OVERLAY:
				renderer.drawOverlay( obj.alpha, obj.pos1, obj.pos2, obj.col);
				break;

            case BUTTON:
#ifdef BLOBBY_MODERN_UI
                {
                    const int size = obj.flags & TF_SMALL_FONT ? FONT_WIDTH_SMALL : FONT_WIDTH_NORMAL;
                    const Vector2 a = obj.pos1 - Vector2(5, 4);
                    const Vector2 b = obj.pos1 + Vector2(TextManager::getUTF8Length(obj.text) * size + 5, size + 4);
                    renderer.drawOverlay(0.94f, a, b, Color(12, 24, 48));
                    if (!mInactive && obj.id == mActiveButton) drawFocusFrame(renderer, a, b);
                    else renderer.drawOverlay(0.8f, Vector2(a.x, b.y - 2), b, Color(65, 90, 125));
                }
                // Focus is evaluated after navigation, so only the final
                // selected widget is highlighted (including when moving up).
                renderer.drawText(obj.text, obj.pos1, obj.flags & ~TF_HIGHLIGHT);
                break;
#endif
			case TEXT:
				renderer.drawText( obj.text, obj.pos1, obj.flags);
				break;

			case SCROLLBAR:
				renderer.drawOverlay( 0.5, obj.pos1, obj.pos1 + Vector2( 210.0, 26.0));
				renderer.drawImage( "gfx/scrollbar.bmp", obj.pos1 + Vector2( obj.pos2.x * 200.0 + 5 , 13));
				break;

			case ACTIVESCROLLBAR:
				renderer.drawOverlay( 0.4, obj.pos1, obj.pos1 + Vector2( 210.0, 26.0));
				renderer.drawImage( "gfx/scrollbar.bmp", obj.pos1 + Vector2( obj.pos2.x * 200.0 + 5 , 13));
				break;

			case EDITBOX:
				FontSize = (obj.flags & TF_SMALL_FONT ? FONT_WIDTH_SMALL : FONT_WIDTH_NORMAL);
				renderer.drawOverlay( 0.5, obj.pos1, obj.pos1 + Vector2( 10 + obj.length * FontSize, 10 + FontSize));
				renderer.drawText( obj.text, obj.pos1 + Vector2( 5, 5), obj.flags);
				break;

			case ACTIVEEDITBOX:
				FontSize = (obj.flags & TF_SMALL_FONT ? FONT_WIDTH_SMALL : FONT_WIDTH_NORMAL);
				renderer.drawOverlay( 0.3, obj.pos1, obj.pos1 + Vector2( 10 + obj.length * FontSize, 10 + FontSize));
				renderer.drawText( obj.text, obj.pos1 + Vector2( 5, 5), obj.flags);
				if (obj.pos2.x >= 0)
					renderer.drawOverlay( 1.0, Vector2((obj.pos2.x) * FontSize + obj.pos1.x + 5, obj.pos1.y + 5), Vector2((obj.pos2.x) * FontSize + obj.pos1.x + 5 + 3, obj.pos1.y + 5 + FontSize), Color( 255, 255, 255));
				break;

			case SELECTBOX:
			case ACTIVESELECTBOX:
				FontSize = (obj.flags & TF_SMALL_FONT ? (FONT_WIDTH_SMALL+LINE_SPACER_SMALL) : (FONT_WIDTH_NORMAL+LINE_SPACER_NORMAL));
				renderer.drawOverlay((obj.type == SELECTBOX ? 0.5 : 0.3), obj.pos1, obj.pos2);
				for (unsigned int c = 0; c < obj.entries.size(); c++)
				{
#ifdef BLOBBY_MODERN_UI
                    if (c == static_cast<unsigned int>(obj.selected))
                    {
                        const Vector2 row(obj.pos1.x + 3, obj.pos1.y + c * FontSize + 3);
                        renderer.drawOverlay(0.95f, row, Vector2(obj.pos2.x - 32, row.y + FontSize), Color(25, 60, 85));
                        renderer.drawOverlay(1.0f, row, row + Vector2(3, FontSize), Color(255, 215, 40));
                    }
#endif
					if( c == static_cast<unsigned int>(obj.selected) )
						renderer.drawText( obj.entries[c], Vector2( obj.pos1.x + 5, obj.pos1.y + (c * FontSize) + 5), obj.flags | TF_HIGHLIGHT);
					else
						renderer.drawText( obj.entries[c], Vector2( obj.pos1.x + 5, obj.pos1.y + (c * FontSize) + 5), obj.flags);
				}
				break;

			case CHAT:
			case ACTIVECHAT:
				FontSize = (obj.flags & TF_SMALL_FONT ? (FONT_WIDTH_SMALL+LINE_SPACER_SMALL) : (FONT_WIDTH_NORMAL+LINE_SPACER_NORMAL));
				renderer.drawOverlay((obj.type == CHAT ? 0.5 : 0.3), obj.pos1, obj.pos2);
				for (unsigned int c = 0; c < obj.entries.size(); c++)
				{
					if (obj.text[c] == 'R' )
						renderer.drawText( obj.entries[c], Vector2( obj.pos1.x + 5, obj.pos1.y + (c * FontSize) + 5), obj.flags | TF_HIGHLIGHT);
					else
						renderer.drawText( obj.entries[c], Vector2( obj.pos1.x + 5, obj.pos1.y + (c * FontSize) + 5), obj.flags);
				}
				break;

			case BLOB:
				renderer.drawBlob( obj.pos1, obj.col);
				break;

			default:
				break;
		}
#ifdef BLOBBY_MODERN_UI
        if (!mInactive && obj.id == mActiveButton)
        {
            switch (obj.type)
            {
                case SCROLLBAR: case ACTIVESCROLLBAR:
                    drawFocusFrame(renderer, obj.pos1 - Vector2(3, 3), obj.pos1 + Vector2(213, 29));
                    break;
                case EDITBOX: case ACTIVEEDITBOX:
                    FontSize = obj.flags & TF_SMALL_FONT ? FONT_WIDTH_SMALL : FONT_WIDTH_NORMAL;
                    drawFocusFrame(renderer, obj.pos1 - Vector2(3, 3), obj.pos1 + Vector2(13 + obj.length * FontSize, 13 + FontSize));
                    break;
                case SELECTBOX: case ACTIVESELECTBOX: case CHAT: case ACTIVECHAT:
                    drawFocusFrame(renderer, obj.pos1 - Vector2(3, 3), obj.pos2 + Vector2(3, 3));
                    break;
                default: break;
            }
        }
#endif
		mQueue->pop();
	}
#ifdef BLOBBY_MODERN_UI
    if (!mMenuItems.empty())
    {
        renderer.drawOverlay(0.97f, Vector2(0, 580), Vector2(800, 600), Color(8, 16, 30));
        std::string hint = "D-pad / stick: move   A: select   B: back   Touch: tap";
        for (const auto& item : mMenuItems) if (item.id == mActiveButton)
        {
            if (mEditing && (item.kind == SELECTBOX || item.kind == CHAT))
                hint = "Up / Down: choose   A / B: done   Touch: tap a row";
            else if (mEditing && item.kind == SCROLLBAR)
                hint = "Left / Right: adjust   A / B: done   Touch: drag";
            else if (mEditing && item.kind == EDITBOX)
                hint = "Type with the keyboard   A / B: done";
            else if (item.kind != BUTTON)
                hint = "D-pad / stick: move   A: adjust   B: back   Touch: tap";
        }
        renderer.drawText(hint, Vector2(16, 584), TF_SMALL_FONT);
    }
#endif
#if BLOBBY_ON_DESKTOP
	if (mDrawCursor)
	{
		renderer.drawImage( "gfx/cursor.bmp", mInputManager->position() + Vector2( 24.0, 24.0));
		mDrawCursor = false;
	}
#endif
	static bool lastCursor = false;
	bool relativeCursor = !mUsingCursor && mInputManager->isMouseCaptured();
	if( relativeCursor != lastCursor  )
	{
		lastCursor = relativeCursor;
		SDL_SetRelativeMouseMode( relativeCursor ? SDL_TRUE : SDL_FALSE );
	}
}

void IMGUI::doImage(int id, const Vector2& position, const std::string& name, const Vector2& size)
{
	QueueObject obj;
	obj.type = IMAGE;
	obj.id = id;
	obj.pos1 = position;
	obj.pos2 = size;
	obj.text = name;
	mQueue->push(obj);
}

void IMGUI::doText(int id, const Vector2& position, const std::string& text, unsigned int flags)
{
	QueueObject obj;
	obj.type = TEXT;
	obj.id = id;
	obj.pos1 = position;

	int const fontSize = flags & TF_SMALL_FONT ? FONT_WIDTH_SMALL : FONT_WIDTH_NORMAL;
	// update position depending on alignment
	if( flags & TF_ALIGN_CENTER )
	{
		obj.pos1.x -= text.size() * fontSize / 2;
	}

	if( flags & TF_ALIGN_RIGHT )
	{
		obj.pos1.x -= text.size() * fontSize;
	}


	obj.text = text;
	obj.flags = flags;
	mQueue->push(obj);
}

void IMGUI::doText(int id, const Vector2& position, TextManager::STRING text, unsigned int flags)
{
	doText(id, position, getText(text), flags);
}

void IMGUI::doOverlay(int id, const Vector2& pos1, const Vector2& pos2, const Color& col, float alpha)
{
	QueueObject obj;
	obj.type = OVERLAY;
	obj.id = id;
	obj.pos1 = pos1;
	obj.pos2 = pos2;
	obj.col = col;
	obj.alpha = alpha;
#ifdef BLOBBY_MODERN_UI
    if (col.r == 0 && col.g == 0 && col.b == 0)
    { obj.col = Color(8, 16, 30); obj.alpha = std::max(alpha, 0.84f); }
#endif
	mQueue->push(obj);
}

bool IMGUI::doButton(int id, const Vector2& position, TextManager::STRING text, unsigned int flags)
{
	return doButton(id, position, getText(text), flags);
}

bool IMGUI::doButton(int id, const Vector2& position, const std::string& text, unsigned int flags)
{
	bool clicked = false;
	QueueObject obj;
	obj.id = id;
	obj.pos1 = position;
	obj.text = text;
#ifdef BLOBBY_MODERN_UI
    obj.type = BUTTON;
#else
    obj.type = TEXT;
#endif
	obj.flags = flags;

	int const textLength = TextManager::getUTF8Length(text);
	int const fontSize = flags & TF_SMALL_FONT ? FONT_WIDTH_SMALL : FONT_WIDTH_NORMAL;

	// update position depending on alignment
	if( flags & TF_ALIGN_CENTER )
	{
		obj.pos1.x -= textLength * fontSize / 2;
	}

	if( flags & TF_ALIGN_RIGHT )
	{
		obj.pos1.x -= textLength * fontSize;
	}
#ifdef BLOBBY_MODERN_UI
    trackMenuItem(id, BUTTON, obj.pos1 - Vector2(5, 5), obj.pos1 + Vector2(textLength * fontSize + 5, fontSize + 5));
#endif


	if (!mInactive)
	{
		// M.W. : Activate cursorless object-highlighting once the up or down key is pressed.
		if (mActiveButton == -1)
		{
			switch (mLastKeyAction)
			{
				case KeyAction::DOWN:
					mActiveButton = 0;
					mLastKeyAction = KeyAction::NONE;
					break;

				case KeyAction::UP:
					mActiveButton = mLastWidget;
					mLastKeyAction = KeyAction::NONE;
					break;

				default:
					break;
			}
		}

		// Highlight first menu object for arrow key navigation.
		if (mActiveButton == 0 && !mButtonReset)
			mActiveButton = id;

		// React to keyboard input.
		if (id == mActiveButton)
		{
			obj.flags = obj.flags | TF_HIGHLIGHT;
			switch (mLastKeyAction)
			{
				case KeyAction::DOWN:
					mActiveButton = 0;
					mLastKeyAction = KeyAction::NONE;
					break;

				case KeyAction::UP:
					mActiveButton = mLastWidget;
					mLastKeyAction = KeyAction::NONE;
					break;

				case KeyAction::SELECT:
					clicked = true;
					mLastKeyAction = KeyAction::NONE;
					break;
				default:
					break;
			}
		}

		// React to back button
		if (mLastKeyAction == KeyAction::BACK)
		{
			if ((text == getText(TextManager::LBL_CANCEL)) ||
                (text == getText(TextManager::LBL_NO))
#ifndef BLOBBY_MODERN_UI
                || (text == getText(TextManager::MNU_LABEL_EXIT))
#endif
                )
			{
				//todo: Workaround to catch backkey
				clicked = true;
				mActiveButton = id;
#ifdef BLOBBY_MODERN_UI
                mLastKeyAction = KeyAction::NONE;
#endif
			}
		}

		#if defined(BLOBBY_MODERN_UI)
            const int tolerance = 4;
		#elif BLOBBY_ON_MOBILE
			const int tolerance = 3;
		#else
			const int tolerance = 0;
		#endif
		// React to mouse input.
		Vector2 mousepos = mInputManager->position();
		if (mousepos.x + tolerance >= obj.pos1.x &&
			mousepos.y + tolerance * 2 >= obj.pos1.y &&
			mousepos.x - tolerance <= obj.pos1.x + textLength * fontSize &&
			mousepos.y - tolerance * 2 <= obj.pos1.y + fontSize)
		{
			obj.flags = obj.flags
			#if BLOBBY_ON_DESKTOP
				| TF_HIGHLIGHT
			#endif
			;
			if (mInputManager->click()
#ifdef BLOBBY_MODERN_UI
                && !mPointerClaimed
#endif
                )
			{
#ifdef BLOBBY_MODERN_UI
                mPointerClaimed = true;
                mEditing = false;
#endif
				clicked = true;
				mActiveButton = id;
#ifdef BLOBBY_MODERN_UI
                mLastKeyAction = KeyAction::NONE;
#endif
			}
		}
	}

	mLastWidget = id;
	mQueue->push(obj);
	return clicked;
}

bool IMGUI::doImageButton(int id, const Vector2& position, const Vector2& size, const std::string& image)
{
	doImage(id, position, image);
#ifdef BLOBBY_MODERN_UI
    trackMenuItem(id, BUTTON, position - size * 0.5f, position + size * 0.5f);
    if (!mInactive && id == mActiveButton && mLastKeyAction == KeyAction::SELECT)
    { mLastKeyAction = KeyAction::NONE; return true; }
#endif

	// React to mouse input.
	if (mInputManager->click())
	{
		Vector2 mousepos = mInputManager->position();
		Vector2 btnpos = position - size * 0.5;
		if (mousepos.x > btnpos.x && mousepos.y > btnpos.y &&
				mousepos.x < btnpos.x + size.x &&	mousepos.y < btnpos.y + size.y)
		{
			return true;
		}
	}

	return false;
}

bool IMGUI::doScrollbar(int id, const Vector2& position, float& value)
{
	QueueObject obj;
	obj.id = id;
	obj.pos1 = position;
	obj.type = SCROLLBAR;
#ifdef BLOBBY_MODERN_UI
    trackMenuItem(id, SCROLLBAR, position, position + Vector2(210, 26));
#endif

	bool deselected = false;

	if (mInputManager->unclick())
	{
		if (id == mHeldWidget)
			deselected = true;

		mHeldWidget = 0;
	}


	if (!mInactive)
	{
		// M.W. : Activate cursorless object-highlighting once the up or down key is pressed.
		if (mActiveButton == -1)
		{
			switch (mLastKeyAction)
			{
				case KeyAction::DOWN:
					mActiveButton = 0;
					mLastKeyAction = KeyAction::NONE;
					break;

				case KeyAction::UP:
					mActiveButton = mLastWidget;
					mLastKeyAction = KeyAction::NONE;
					break;

				default:
					break;
			}
		}

		// Highlight first menu object for arrow key navigation.
		if (mActiveButton == 0 && !mButtonReset)
			mActiveButton = id;

		// React to keyboard input.
		if (id == mActiveButton)
		{
			obj.type = ACTIVESCROLLBAR;
			switch (mLastKeyAction)
			{
				case KeyAction::DOWN:
					mActiveButton = 0;
					mLastKeyAction = KeyAction::NONE;
					break;

				case KeyAction::UP:
					mActiveButton = mLastWidget;
					mLastKeyAction = KeyAction::NONE;
					break;

				case KeyAction::LEFT:
					value -= 0.1;
					mLastKeyAction = KeyAction::NONE;
					break;

				case KeyAction::RIGHT:
					value += 0.1;
					mLastKeyAction = KeyAction::NONE;
					break;

				default:
					break;
			}
		}

		#if defined(BLOBBY_MODERN_UI)
            const int tolerance = 4;
		#elif BLOBBY_ON_MOBILE
			const int tolerance = 3;
		#else
			const int tolerance = 0;
		#endif

		// React to mouse input.
		Vector2 mousepos = mInputManager->position();
		if (mousepos.x + 5 > position.x &&
			mousepos.y + tolerance * 2 > position.y &&
			mousepos.x < position.x + 205 &&
			mousepos.y - tolerance < position.y + 24.0)
		{
			obj.type = ACTIVESCROLLBAR;

			if (mInputManager->click())
			{
				mHeldWidget = id;
			}

			if (mHeldWidget == id)
			{
				value = (mousepos.x - position.x) / 200.0;
				mActiveButton = id;
			}

			if(mInputManager->mouseWheelUp())
				value += 0.1;

			if(mInputManager->mouseWheelDown())
				value -= 0.1;
		}
	}

#ifdef BLOBBY_MODERN_UI
    if (mHeldWidget == id && !mInactive)
        value = (mInputManager->position().x - position.x) / 200.0f;
    if (mInputManager->unclick()) mHeldWidget = 0;
#endif
	value = value > 0.f ? (value < 1.f ? value : 1.f) : 0.f;
	obj.pos2.x = value;

	mLastWidget = id;
	mQueue->push(obj);

	return deselected;
}

void IMGUI::resetSelection()
{
	mInactive = false;
	mActiveButton = -1;
	mButtonReset = true;
#ifdef BLOBBY_MODERN_UI
    mMenuItems.clear();
    mPreviousMenuItems.clear();
    mEditing = false;
    mLastKeyAction = KeyAction::NONE;
    mPointerClaimed = true;
    SDL_StopTextInput();
#endif
}

bool IMGUI::doEditbox(int id, const Vector2& position, unsigned int length, std::string& text, unsigned& cpos, unsigned int flags, bool force_active)
{
	int FontSize = (flags & TF_SMALL_FONT ? FONT_WIDTH_SMALL : FONT_WIDTH_NORMAL);
	bool changed = false;
	QueueObject obj;
	obj.id = id;
	obj.pos1 = position;
	obj.type = EDITBOX;
	obj.length = length; // length does not actually work!
	obj.flags = flags;

	// Width and height including border
	int width = length * FontSize + 10;
	int height = FontSize + 10;

		// update position depending on alignment
	if( flags & TF_ALIGN_CENTER )
	{
		obj.pos1.x -= width / 2;
	}

#ifdef BLOBBY_MODERN_UI
    trackMenuItem(id, EDITBOX, obj.pos1, obj.pos1 + Vector2(width, height));
    if (cpos > text.size()) cpos = text.size();
#endif
	// React to mouse input.
	Vector2 mousepos = mInputManager->position();
	if (mousepos.x > obj.pos1.x &&
		mousepos.y > obj.pos1.y &&
		mousepos.x < obj.pos1.x + width &&
		mousepos.y < obj.pos1.y + height)
	{
		obj.flags = obj.flags | TF_HIGHLIGHT;
		if (mInputManager->click())
		{
			// Handle click on the text.
			if (mousepos.x < obj.pos1.x + text.length() * FontSize)
				cpos = (int) ((mousepos.x-obj.pos1.x-5+(FontSize/2)) / FontSize);
			// Handle click behind the text.
			else if (mousepos.x < obj.pos1.x + width)
				cpos = (int) text.length();

			mActiveButton = id;

#ifdef BLOBBY_MODERN_UI
            mEditing = true;
#endif
			// Show keyboard
			SDL_StartTextInput();
		}
	}


	if (!mInactive)
	{
		// M.W. : Activate cursorless object-highlighting once the up or down key is pressed.
		if (mActiveButton == -1)
		{
			switch (mLastKeyAction)
			{
				case KeyAction::DOWN:
					mActiveButton = 0;
					mLastKeyAction = KeyAction::NONE;
					break;

				case KeyAction::UP:
					mActiveButton = mLastWidget;
					mLastKeyAction = KeyAction::NONE;
					break;

				default:
					break;
			}

			// M.W. : Initialize the cursor position at the end of the string.
			// IMPORTANT: If you make changes to EditBox text that alter the length
			//				of the text, either call resetSelection() to come back
			//				to this area of code or update cpos manually to prevent
			//				crashes due to a misplaced cursor.
			cpos = text.length();
		}

		// Highlight first menu object for arrow key navigation.
		if (mActiveButton == 0 && !mButtonReset)
			mActiveButton = id;

		// React to keyboard input.
		if ((id == mActiveButton
#ifdef BLOBBY_MODERN_UI
            && mEditing
#endif
            ) || force_active)
		{
			obj.type = ACTIVEEDITBOX;
			switch (mLastKeyAction)
			{
				case KeyAction::DOWN:
					mActiveButton = 0;
					mLastKeyAction = KeyAction::NONE;
					break;

				case KeyAction::UP:
					mActiveButton = mLastWidget;
					mLastKeyAction = KeyAction::NONE;
					break;

				case KeyAction::LEFT:
					if (cpos > 0)
						cpos--;
					mLastKeyAction = KeyAction::NONE;
					break;

				case KeyAction::RIGHT:
					if (cpos < text.length())
						cpos++;
					mLastKeyAction = KeyAction::NONE;
					break;

				default:
					break;
			}
			std::string input = mInputManager->getLastTextKey();

			if (input == "backspace" && text.length() > 0 && cpos > 0)
			{
				text.erase(cpos - 1, 1);
				cpos--;
			}
			else if (input == "del" && text.length() > cpos)
			{
				text.erase(cpos, 1);
			}
			else if (input == "return")
			{
				// Workarround for chatwindow! Delete this after GUI-Rework
				changed = true;
			}
			// This is a temporary solution until the new
			// UTF-8 class can tell the real length!!!
			else if (text.length() < length)
			{
				if (input == "paste")
				{
					char * clipboardText = SDL_GetClipboardText();

					if (clipboardText != nullptr) {
						for (int i = 0; text.length() < length; i++)
						{
							const char c = clipboardText[i];

							// Only ASCII is currently supported
							if ((c == '\0') || (c < 0x20) || (c > 0xf7))
								break;

							text.insert(cpos, &c, 1);
							cpos++;
							changed = true;
						}

						SDL_free(clipboardText);
					}
				}
				if (input == "space")
				{
					text.insert(cpos, " ");
					cpos++;
					changed = true;
				}
				else if (input == "keypad0")
				{
					text.insert(cpos, "0");
					cpos++;
					changed = true;
				}
				else if (input == "keypad1")
				{
					text.insert(cpos, "1");
					cpos++;
					changed = true;
				}
				else if (input == "keypad2")
				{
					text.insert(cpos, "2");
					cpos++;
					changed = true;
				}
				else if (input == "keypad3")
				{
					text.insert(cpos, "3");
					cpos++;
					changed = true;
				}
				else if (input == "keypad4")
				{
					text.insert(cpos, "4");
					cpos++;
					changed = true;
				}
				else if (input == "keypad5")
				{
					text.insert(cpos, "5");
					cpos++;
					changed = true;
				}
				else if (input == "keypad6")
				{
					text.insert(cpos, "6");
					cpos++;
					changed = true;
				}
				else if (input == "keypad7")
				{
					text.insert(cpos, "7");
					cpos++;
					changed = true;
				}
				else if (input == "keypad8")
				{
					text.insert(cpos, "8");
					cpos++;
					changed = true;
				}
				else if (input == "keypad9")
				{
					text.insert(cpos, "9");
					cpos++;
					changed = true;
				}
				else if (input.length() == 1)
				{
					text.insert(cpos, input);
					cpos++;
					changed = true;
				}
			}
		}
	}

	obj.pos2.x = SDL_GetTicks() % 1000 >= 500 ? cpos : -1.0;
	obj.text = text;

	mLastWidget = id;
	mQueue->push(obj);

	// when content changed, it is active
	// part of chat window hack
	if( changed && force_active )
		mActiveButton = id;

	return changed;
}

SelectBoxAction IMGUI::doSelectbox(int id, const Vector2& pos1, const Vector2& pos2, const std::vector<std::string>& entries, unsigned int& selected, unsigned int flags)
{
	int FontSize = (flags & TF_SMALL_FONT ? (FONT_WIDTH_SMALL+LINE_SPACER_SMALL) : (FONT_WIDTH_NORMAL+LINE_SPACER_NORMAL));
	SelectBoxAction changed = SBA_NONE;
	QueueObject obj;
	obj.id = id;
	obj.pos1 = pos1;
	obj.pos2 = pos2;
	obj.type = SELECTBOX;
#ifdef BLOBBY_MODERN_UI
    trackMenuItem(id, SELECTBOX, pos1, pos2);
#endif
	obj.flags = flags;

	const int itemsPerPage = int(pos2.y - pos1.y - 10) / FontSize;
	int first = (int)(selected / itemsPerPage)*itemsPerPage; //the first visible element in the list


	if (!mInactive)
	{
		// M.W. : Activate cursorless object-highlighting once the up or down key is pressed.
		if (mActiveButton == -1)
		{
			switch (mLastKeyAction)
			{
				case KeyAction::DOWN:
					mActiveButton = 0;
					mLastKeyAction = KeyAction::NONE;
					break;

				case KeyAction::UP:
					mActiveButton = mLastWidget;
					mLastKeyAction = KeyAction::NONE;
					break;

				default:
					break;
			}
		}

		// Highlight first menu object for arrow key navigation.
		if (mActiveButton == 0 && !mButtonReset)
			mActiveButton = id;

		// React to keyboard input.
		if (id == mActiveButton)
		{
			obj.type = ACTIVESELECTBOX;
			switch (mLastKeyAction)
			{
				case KeyAction::DOWN:
					mActiveButton = 0;
					mLastKeyAction = KeyAction::NONE;
					break;

				case KeyAction::UP:
					mActiveButton = mLastWidget;
					mLastKeyAction = KeyAction::NONE;
					break;

				case KeyAction::LEFT:
					if (selected > 0)
					{
						selected--;
						changed = SBA_SELECT;
					}
					mLastKeyAction = KeyAction::NONE;
					break;

				case KeyAction::RIGHT:
					if (selected + 1 < entries.size())
					{
						selected++;
						changed = SBA_SELECT;
					}
					mLastKeyAction = KeyAction::NONE;
					break;

				default:
					break;
			}
		}

		// React to mouse input.
		Vector2 mousepos = mInputManager->position();
		if (mousepos.x > pos1.x && mousepos.y > pos1.y && mousepos.x < pos2.x && mousepos.y < pos2.y)
		{
			obj.type = ACTIVESELECTBOX;
			if (mInputManager->click())
				mActiveButton = id;
		}
		//entries mouseclick:
		if (mousepos.x > pos1.x &&
			mousepos.y > pos1.y+5 &&
			mousepos.x < pos2.x-35 &&
			mousepos.y < pos1.y+5+FontSize*itemsPerPage)
		{
			if (mInputManager->click())
			{
				int tmp = (int)((mousepos.y - pos1.y - 5) / FontSize) + first;
				/// \todo well, it's not really a doulbe click...
				/// we need to do this in inputmanager
				if( selected == static_cast<unsigned int>(tmp) && mInputManager->doubleClick() )
					changed = SBA_DBL_CLICK;

				if (tmp >= 0 && static_cast<unsigned int>(tmp) < entries.size())
					selected = tmp;

				mActiveButton = id;
			}
			if ((mInputManager->mouseWheelUp()) && (selected > 0))
			{
				selected--;
				changed = SBA_SELECT;
			}
			if ((mInputManager->mouseWheelDown()) && (selected + 1 < entries.size()))
			{
				selected++;
				changed = SBA_SELECT;
			}
		}
		//arrows mouseclick:
		if (mousepos.x > pos2.x-30 && mousepos.x < pos2.x-30+24 && mInputManager->click())
		{
			if (mousepos.y > pos1.y+3 && mousepos.y < pos1.y+3+24 && selected > 0)
			{
				selected--;
				changed = SBA_SELECT;
			}

			if (mousepos.y > pos2.y-27 && mousepos.y < pos2.y-27+24 && selected + 1 < entries.size())
			{
				selected++;
				changed = SBA_SELECT;
			}
		}
	}
	doImage(getNextId(), Vector2(pos2.x-15, pos1.y+15), "gfx/pfeil_oben.bmp");
	doImage(getNextId(), Vector2(pos2.x-15, pos2.y-15), "gfx/pfeil_unten.bmp");

	first = (selected / itemsPerPage)*itemsPerPage; //recalc first
	if ( !entries.empty() )
	{
		unsigned int last = first + itemsPerPage;
		if (last > entries.size())
			last = entries.size();

		obj.entries = std::vector<std::string>(entries.begin()+first, entries.begin()+last);
	}
	else
		obj.entries = std::vector<std::string>();

	obj.selected = selected-first;

	mLastWidget = id;
	mQueue->push(obj);

	return changed;
}

void IMGUI::doChatbox(int id, const Vector2& pos1, const Vector2& pos2, const std::vector<std::string>& entries, unsigned int& selected, const std::vector<bool>& local, unsigned int flags)
{
	assert( entries.size() == local.size() );
	int FontSize = (flags & TF_SMALL_FONT ? (FONT_WIDTH_SMALL+LINE_SPACER_SMALL) : (FONT_WIDTH_NORMAL+LINE_SPACER_NORMAL));
	QueueObject obj;
	obj.id = id;
	obj.pos1 = pos1;
	obj.pos2 = pos2;
	obj.type = CHAT;
#ifdef BLOBBY_MODERN_UI
    trackMenuItem(id, CHAT, pos1, pos2);
#endif
	obj.flags = flags;

	const unsigned int itemsPerPage = int(pos2.y - pos1.y - 10) / FontSize;


	if (!mInactive)
	{
		// M.W. : Activate cursorless object-highlighting once the up or down key is pressed.
		if (mActiveButton == -1)
		{
			switch (mLastKeyAction)
			{
				case KeyAction::DOWN:
					mActiveButton = 0;
					mLastKeyAction = KeyAction::NONE;
					break;

				case KeyAction::UP:
					mActiveButton = mLastWidget;
					mLastKeyAction = KeyAction::NONE;
					break;

				default:
					break;
			}
		}

		// Highlight first menu object for arrow key navigation.
		if (mActiveButton == 0 && !mButtonReset)
			mActiveButton = id;

		// React to keyboard input.
		if (id == mActiveButton)
		{
			obj.type = ACTIVECHAT;
			switch (mLastKeyAction)
			{
				case KeyAction::DOWN:
					mActiveButton = 0;
					mLastKeyAction = KeyAction::NONE;
					break;

				case KeyAction::UP:
					mActiveButton = mLastWidget;
					mLastKeyAction = KeyAction::NONE;
					break;

				case KeyAction::LEFT:
					if (selected > 0)
					{
						selected--;
					}
					mLastKeyAction = KeyAction::NONE;
					break;

				case KeyAction::RIGHT:
					if (selected + 1 < entries.size())
					{
						selected++;
					}
					mLastKeyAction = KeyAction::NONE;
					break;

				default:
					break;
			}
		}

		// React to mouse input.
		Vector2 mousepos = mInputManager->position();
		if (mousepos.x > pos1.x && mousepos.y > pos1.y && mousepos.x < pos2.x && mousepos.y < pos2.y)
		{
			if (mInputManager->click())
				mActiveButton = id;
		}
		//entries mouseclick:
		if (mousepos.x > pos1.x &&
			mousepos.y > pos1.y+5 &&
			mousepos.x < pos2.x-35 &&
			mousepos.y < pos1.y+5+FontSize*itemsPerPage)
		{
			if ((mInputManager->mouseWheelUp()) && (selected > 0))
			{
				selected--;
			}

			if ((mInputManager->mouseWheelDown()) && (selected + 1 < entries.size()))
			{
				selected++;
			}
		}
		//arrows mouseclick:
		if (mousepos.x > pos2.x-30 && mousepos.x < pos2.x-30+24 && mInputManager->click())
		{
			if (mousepos.y > pos1.y+3 && mousepos.y < pos1.y+3+24 && selected > 0)
			{
				selected--;
			}

			if (mousepos.y > pos2.y-27 && mousepos.y < pos2.y-27+24 && selected + 1 < entries.size())
			{
				selected++;
			}
		}
	}
	doImage(getNextId(), Vector2(pos2.x-15, pos1.y+15), "gfx/pfeil_oben.bmp");
	doImage(getNextId(), Vector2(pos2.x-15, pos2.y-15), "gfx/pfeil_unten.bmp");

	unsigned int first = (selected / itemsPerPage) * itemsPerPage; //recalc first
	if ( !entries.empty() )
	{
		unsigned int last = selected + 1;
		/// \todo maybe we should adapt selected so we even can't scroll up further!
		// we don't want negative chatlog, so we just scroll upward without coming to negative
		// elements.
		if (last >= itemsPerPage)
			first = last - itemsPerPage;
		else
			first = 0;

		// check that we don't create out of bounds problems
		if(last > entries.size())
		{
			last = entries.size();
		}

		obj.entries = std::vector<std::string>(entries.begin()+first, entries.begin()+last);
		// HACK: we use taxt to store information which text is from local player and which from
		//			remote player.
		obj.text = "";
		for(unsigned int i = first; i < last; ++i)
		{
			obj.text += local[i] ? 'L' : 'R';
		}
	}
	else
		obj.entries = std::vector<std::string>();

	obj.selected = selected-first;

	mLastWidget = id;
	mQueue->push(obj);
}


bool IMGUI::doBlob(int id, const Vector2& position, const Color& col)
{
	QueueObject obj;
	obj.id = id;
	obj.pos1 = position;
	obj.type = BLOB;
	obj.col = col;
	mQueue->push(obj);
	return false;
}

void IMGUI::doCursor(bool draw)
{
	mDrawCursor = draw;
	mUsingCursor = true;
}

bool IMGUI::usingCursor() const
{
	return mUsingCursor;
}

const TextManager& IMGUI::textMgr() const {
	return *mTextManager;
}

const std::string& IMGUI::getText(TextManager::STRING id) const {
	return textMgr().getString(id);
}

void IMGUI::setTextMgr(std::string lang) {
	mTextManager.reset(new TextManager(std::move(lang)));
}

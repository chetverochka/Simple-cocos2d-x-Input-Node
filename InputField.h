#pragma once

#include "cocos2d.h"
#include "ui/UIScale9Sprite.h"
#include "ui/CocosGUI.h"

namespace textInputDefault {
	std::string bgPath = "Square.png";
	cocos2d::Rect bgRect = cocos2d::Rect(30, 30, 70, 70);
	std::string fontPath = "fonts/arial.ttf";
	cocos2d::Color3B placeholderColor = cocos2d::Color3B::GRAY;
	cocos2d::Color3B defaultTextColor = cocos2d::Color3B::WHITE;
}


class InputField : public cocos2d::Node {

	cocos2d::ui::Scale9Sprite* bg;
	cocos2d::Label* placeholder;
	std::string placeholderString;
	cocos2d::ui::TextField * field;
	std::function<void(cocos2d::Ref* sender, cocos2d::ui::TextField::EventType type)> callback;

	bool init(std::string _placeholderString, cocos2d::Size size) {
		if (!cocos2d::Node::init()) {
			return false;
		}

		placeholderString = _placeholderString;
		callback = nullptr;

		initBackground();
		initField();
		initPlaceholder();

		cocos2d::EventListenerTouchOneByOne* listener = cocos2d::EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(true);
		listener->onTouchBegan = CC_CALLBACK_2(InputField::touchIsDown, this);
		listener->onTouchEnded = [this](cocos2d::Touch* t, cocos2d::Event* e) {};

		cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

		this->setContentSize(size);
		this->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));
		return true;
	}

	void initBackground() {
		bg = cocos2d::ui::Scale9Sprite::create(textInputDefault::bgPath);
		bg->setScale9Enabled(true);
		bg->setCapInsets(textInputDefault::bgRect);
		bg->setColor(cocos2d::Color3B::BLACK);
		bg->setOpacity(120);
		this->addChild(bg);
	}

	void initPlaceholder() {
		placeholder = cocos2d::Label::createWithTTF(field->getPlaceHolder(), textInputDefault::fontPath, 50);
		placeholder->setColor(textInputDefault::placeholderColor);
		this->addChild(placeholder, 1);
	}

	void initField() {
		field = cocos2d::ui::TextField::create(placeholderString, textInputDefault::fontPath, 0);
		field->addEventListener(CC_CALLBACK_2(InputField::fieldCallback, this));
		bg->addChild(field, 100);
	}

	void setLabelSize(cocos2d::Label* label) {
		float maxHeight = 0.8;
		float maxWidth = 0.95;
		cocos2d::Size size = this->getContentSize();

		cocos2d::TTFConfig config;
		config = label->getTTFConfig();
		config.fontSize = size.height * maxHeight;
		label->setTTFConfig(config);

		while (label->getBoundingBox().size.width > size.width * maxWidth) {
			cocos2d::TTFConfig config;
			config = label->getTTFConfig();
			config.fontSize -= 0.1;
			label->setTTFConfig(config);
		}
	}

	cocos2d::Rect getRect() {
		cocos2d::Rect rect;

		rect.size = this->getContentSize();
		rect.origin.x = this->getPosition().x - rect.size.width / 2;
		rect.origin.y = this->getPosition().y + rect.size.height / 2;

		return rect;
	}
	cocos2d::Vec2 getPointInWorldSpace(cocos2d::Vec2 pos) {
		cocos2d::Vec2 centerPosition = cocos2d::Vec2(cocos2d::Director::getInstance()->getVisibleSize().width / 2, cocos2d::Director::getInstance()->getVisibleSize().height / 2);
		cocos2d::Vec2 cameraPosition = cocos2d::Director::getInstance()->getRunningScene()->getDefaultCamera()->getPosition();

		cocos2d::Vec2 cameraShift;
		cameraShift.x = centerPosition.x - cameraPosition.x;
		cameraShift.y = centerPosition.y - cameraPosition.y;

		return cocos2d::Vec2(pos.x - cameraShift.x, pos.y - cameraShift.y);
	}
	bool touchIsDown(cocos2d::Touch* touch, cocos2d::Event* event) {
		cocos2d::Vec2 touchPosInGlobalSpace = getPointInWorldSpace(touch->getLocation());
		cocos2d::Vec2 convertedTouchPos = this->convertToNodeSpace(touchPosInGlobalSpace);
		if (bg->getBoundingBox().containsPoint(convertedTouchPos)) {
			field->attachWithIME();
		}
		return true;
	}
	void fieldCallback(cocos2d::Ref* sender, cocos2d::ui::TextField::EventType type) {
		cocos2d::ui::TextField* ref = (cocos2d::ui::TextField*)(sender);
		if (type == cocos2d::ui::TextField::EventType::INSERT_TEXT) {
			placeholder->setString(ref->getString());

			if (placeholder->getColor() != textInputDefault::defaultTextColor) {
				placeholder->setColor(textInputDefault::defaultTextColor);
			}

			setLabelSize(placeholder);
		}
		if (type == cocos2d::ui::TextField::EventType::DELETE_BACKWARD) {
			
			if (ref->getString().size() > 0) {
				placeholder->setString(ref->getString());
				setLabelSize(placeholder);
			}
			else {
				placeholder->setString(ref->getPlaceHolder());
				placeholder->setColor(textInputDefault::placeholderColor);
				setLabelSize(placeholder);
			}
		}

		if (callback) {
			callback(sender, type);
		}
	}
public:
	
	void setContentSize(const cocos2d::Size& size) override {
		cocos2d::Node::setContentSize(size);

		bg->setContentSize(size);
		bg->setPosition(size / 2);

		setLabelSize(placeholder);
		placeholder->setPosition(size / 2);
		field->setPosition(size / 2);
	}
	static InputField* create(std::string placeholder = "Enter text...", cocos2d::Size size = cocos2d::Size(50, 50)) {
		InputField* pRet = new(std::nothrow) InputField;

		if (pRet && pRet->init(placeholder, size)) {
			pRet->autorelease();
			return pRet;
		}
		else {
			CC_SAFE_DELETE(pRet);
			return nullptr;
		}
	}
	void setCallback(std::function<void(cocos2d::Ref* sender, cocos2d::ui::TextField::EventType type)> function) {
		callback = function;
	}
	cocos2d::ui::TextField* getField() {
		return field;
	}
	cocos2d::ui::Scale9Sprite* getSprite() {
		return bg;
	}
	cocos2d::Label* getLabel() {
		return placeholder;
	}

};

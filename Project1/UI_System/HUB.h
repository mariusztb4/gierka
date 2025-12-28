#pragma once
#include "UIRenderer.h"
#include "../Text.h"

class HUB
{
public:
	HUB(UIRenderer* uiRenderer);
	HUB() = default; // Dodaj domyœlny konstruktor

	void currentHP2(unsigned int hp);
	void maxHP2(unsigned int hp);
	void currentMana2(unsigned int mana);
	void maxMana2(unsigned int mana);
	void SetHP(unsigned int current, unsigned int max);
	void SetMana(unsigned int current, unsigned int max);
	void DrawUI();
	void DrawText();
	void Update(float dt);

	// Ustawienia regeneracji
	void SetHPRegen(float hpPerSecond) { m_hpRegenRate = hpPerSecond; }
	void SetManaRegen(float manaPerSecond) { m_manaRegenRate = manaPerSecond; }
	void EnableRegen(bool hp, bool mana) { m_hpRegenEnabled = hp; m_manaRegenEnabled = mana; }

private:
	UIRenderer* m_uiRenderer;
	Text* m_textRenderer;

	unsigned int currentHP;
	unsigned int maxHP;
	unsigned int currentMana;
	unsigned int maxMana;

	// Wartoœci wyœwietlane (p³ynna interpolacja)
	float displayHP;
	float displayMana;

	// Regeneracja
	float m_hpRegenRate = 1.0f;
	float m_manaRegenRate = 1.0f;
	bool m_hpRegenEnabled = true;
	bool m_manaRegenEnabled = true;
};
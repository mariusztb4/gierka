#include "HUB.h"
#include "../Engine.h"

HUB::HUB(UIRenderer* uiRenderer)
	: m_uiRenderer(uiRenderer), currentHP(75), maxHP(100), currentMana(40), maxMana(100),
	  displayHP(75.0f), displayMana(40.0f)
{
}

void HUB::currentHP2(unsigned int hp)
{
	currentHP = hp;
}

void HUB::maxHP2(unsigned int hp)
{
	maxHP = hp;
}

void HUB::currentMana2(unsigned int mana)
{
	currentMana = mana;
}

void HUB::maxMana2(unsigned int mana)
{
	maxMana = mana;
}

void HUB::SetHP(unsigned int current, unsigned int max)
{
	currentHP = current;
	maxHP = max;
}

void HUB::SetMana(unsigned int current, unsigned int max)
{
	currentMana = current;
	maxMana = max;
}

void HUB::DrawUI()
{
	if (m_uiRenderer)
	{
		m_uiRenderer->Begin();
		
		// Użyj wartości wyświetlanych (płynne) zamiast rzeczywistych
		float hpPercent = displayHP / (float)maxHP;
		float manaPercent = displayMana / (float)maxMana;

		// Health Bar
		m_uiRenderer->DrawRect(20, 950, 500, 20, glm::vec4(1, 1, 1, 0.25f));              // tło HP
		m_uiRenderer->DrawRect(20, 950, 500 * hpPercent, 20, glm::vec4(1, 1, 1, 0.5f));  // HP

		// Mana Bar
		m_uiRenderer->DrawRect(20, 980, 500, 20, glm::vec4(1, 1, 1, 0.25f));              // tło Mana
		m_uiRenderer->DrawRect(20, 980, 500 * manaPercent, 20, glm::vec4(1, 1, 1, 0.5f));// Mana

		m_uiRenderer->End();
	}
}

void HUB::DrawText()
{
	if (m_uiRenderer)
	{
		int screenH = Engine::GetInstance(1920, 1080).GetScreenHeight();

		glDisable(GL_SCISSOR_TEST);
		glDisable(GL_DEPTH_TEST);

		m_textRenderer = &Engine::GetInstance(1920, 1080).GetText();
		m_textRenderer->Begin();
		
		// Formatuj z jednym miejscem po przecinku dla wartości wyświetlanych
		char hpBuffer[64];
		char manaBuffer[64];
		snprintf(hpBuffer, sizeof(hpBuffer), "HP: %.1f", displayHP, maxHP);
		snprintf(manaBuffer, sizeof(manaBuffer), "MP: %.1f", displayMana, maxMana);
		
		std::string hpText = hpBuffer;
		std::string manaText = manaBuffer;
		
		float scale = 1.0f;
		float barWidth = 500.0f;
		float barX = 20.0f;
		float barHeight = 20.0f;
		float barCenterX = barX + barWidth / 2.0f;
		
		float hpBarY = 950.0f;
		float manaBarY = 980.0f;
		
		float hpTextWidth = m_textRenderer->GetTextWidth(hpText, scale);
		float manaTextWidth = m_textRenderer->GetTextWidth(manaText, scale);
		
		float textHeight = 24.0f * scale;
		
		float hpX = barCenterX - (hpTextWidth / 2.0f);
		float manaX = barCenterX - (manaTextWidth / 2.0f);
		
		float hpY = screenH - (hpBarY + barHeight / 2.0f + textHeight / 2.0f);
		float manaY = screenH - (manaBarY + barHeight / 2.0f + textHeight / 2.0f);
		
		// Parametry cienia - ZMIENIONE: bardziej przezroczyste i rozmyte
		float shadowOffsetX = 2.0f;  // Zmniejszony offset
		float shadowOffsetY = 2.0f;  // Zmniejszony offset
		glm::vec3 shadowColor = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 textColor = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 outlineColor = glm::vec3(0.0f, 0.0f, 0.0f);
		
		// CIEŃ HP - rozproszony (więcej warstw = bardziej rozmyty)
		for (int i = 5; i >= 1; i--)  // Zwiększono z 3 do 5 warstw
		{
			float offsetMult = i / 5.0f;
			float alpha = 0.08f / i;  // ZMIENIONE: z 0.2f na 0.08f (dużo bardziej przezroczyste)
			
			m_textRenderer->RenderText(
				hpText,
				hpX + shadowOffsetX * offsetMult,
				hpY - shadowOffsetY * offsetMult,
				scale,
				glm::vec4(shadowColor, alpha)
			);
		}
		
		// Tekst HP z obramowaniem
		m_textRenderer->RenderTextOutlined(
			hpText,
			hpX,
			hpY,
			scale,
			textColor,
			outlineColor,
			1.0f
		);
		
		// CIEŃ MANA - rozproszony
		for (int i = 5; i >= 1; i--)  // Zwiększono z 3 do 5 warstw
		{
			float offsetMult = i / 5.0f;
			float alpha = 0.08f / i;  // ZMIENIONE: z 0.2f na 0.08f
			
			m_textRenderer->RenderText(
				manaText,
				manaX + shadowOffsetX * offsetMult,
				manaY - shadowOffsetY * offsetMult,
				scale,
				glm::vec4(shadowColor, alpha)
			);
		}
		
		// Tekst Mana z obramowaniem
		m_textRenderer->RenderTextOutlined(
			manaText,
			manaX,
			manaY,
			scale,
			textColor,
			outlineColor,
			1.0f
		);
		
		m_textRenderer->End();
	}
}

void HUB::Update(float dt)
{
	// ===== PŁYNNA REGENERACJA BEZPOŚREDNIO NA displayHP/displayMana =====
	
	// Płynna regeneracja HP - 1% maxHP na sekundę
	if (m_hpRegenEnabled && displayHP < (float)maxHP)
	{
		float regenAmount = (maxHP * 0.01f) * dt;  // 1% maxHP per frame
		displayHP += regenAmount;
		
		// Ogranicz do maxHP
		if (displayHP > (float)maxHP)
			displayHP = (float)maxHP;
		
		// Aktualizuj rzeczywistą wartość (zaokrąglona)
		currentHP = (unsigned int)displayHP;
	}

	// Płynna regeneracja Mana - 1% maxMana na sekundę
	if (m_manaRegenEnabled && displayMana < (float)maxMana)
	{
		float regenAmount = (maxMana * 0.01f) * dt;  // 1% maxMana per frame
		displayMana += regenAmount;
		
		// Ogranicz do maxMana
		if (displayMana > (float)maxMana)
			displayMana = (float)maxMana;
		
		// Aktualizuj rzeczywistą wartość (zaokrąglona)
		currentMana = (unsigned int)displayMana;
	}
}
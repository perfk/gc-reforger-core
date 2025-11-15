modded class SCR_WeaponInfo // fix for the weapon ui on screen in spectator
{
    override protected void DisplayOnResumed()
    {
        super.DisplayOnResumed();
        
        Show(m_WeaponState && m_WeaponState.m_Weapon, UIConstants.FADE_RATE_SLOW);
    }
}
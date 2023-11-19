//using System.Collections;
//using System.Collections.Generic;
//using static System.Net.Mime.MediaTypeNames;
//using static System.Runtime.CompilerServices.RuntimeHelpers;
//using System.Drawing;
//using System.Numerics;
//using System;

//public class CharacterStats : MonoBehaviour
//{
//    private GameManager gameManager;
//    private BattleManager battleManager;
//    private _StatusEffectHud statusEffectHud;
//    [HideInInspector] public Animator animator;

//    [Header("HUD")]
//    public Sprite uniqueCharacterAvatar;
//    public Transform uniqueTurnHud;
//    public GameObject characterHpHud;
//    public GameObject statusEffectPanel;
//    [SerializeField] private Image healthBarFill;
//    [SerializeField] private GameObject characterName;
//    [SerializeField] private GameObject floatingText;
//    [SerializeField] private GameObject statusEffectText;
//    [HideInInspector] public bool hoverHudCheck;

//    [Header("Other Player HUD")]
//    [SerializeField] private TextMeshProUGUI hpValueUi;
//    public Image playerAvatar;
//    [HideInInspector] public Color32 playerAvatarOriginalColor;
//    [HideInInspector] public Color32 playerAvatarTargetColor;

//    [Header("Other Enemy HUD")]
//    public Image healthPanel;
//    [SerializeField] private TextMeshProUGUI hpPercentUi;

//    [Header("Stats")]
//    public float maxHealth;
//    public float health;
//    public float attack;
//    public float defense;
//    public int speed;

//    [HideInInspector] public float increasedAttackValue;
//    [HideInInspector] public float decreasedDefenseValue;
//    [HideInInspector] public float increasedDefenseValue;

//    [Header("Debuffs")]
//    public int bleedStack;
//    public int shatterCounter;
//    public int stunCounter;
//    public int tauntCounter;

//    [Header("Buffs")]
//    public bool speedCheck;
//    public int enrageCounter;
//    public int armorCounter;

//    [HideInInspector] public bool checkedStatus;

//    private void Start()
//    {
//        gameManager = FindObjectOfType<GameManager>();
//        battleManager = FindObjectOfType<BattleManager>();
//        statusEffectHud = FindObjectOfType<_StatusEffectHud>();
//        animator = GetComponent<Animator>();

//        #region HUD
//        health = maxHealth;
//        healthBarFill.fillAmount = health / maxHealth;
//        NameCheck();
//        hoverHudCheck = false;
//        #endregion

//        #region Other Player HUD
//        if (hpValueUi != null) hpValueUi.text = health.ToString();
//        if (playerAvatar != null)
//        {
//            playerAvatarOriginalColor = playerAvatar.color;
//            playerAvatarTargetColor = new Color32(120, 255, 120, 255);
//        }
//        #endregion

//        #region Other Enemy HUD
//        if (hpPercentUi != null)
//        {
//            hpPercentUi.text = Mathf.RoundToInt(health / maxHealth * 100).ToString() + "%";
//        }
//        #endregion

//        checkedStatus = false;
//    }

//    private void Update()
//    {
//        if (battleManager.battleState != BattleState.NEWGAME)
//        {
//            #region Temp Cheat Code
//            if (Input.GetKeyUp(KeyCode.F10) && gameObject.CompareTag("Enemy"))
//            {
//                health = 1;
//                if (hpPercentUi != null) hpPercentUi.text = Mathf.RoundToInt(health / maxHealth * 100).ToString() + "%";
//                healthBarFill.fillAmount = health / maxHealth;
//            }
//            #endregion

//            if (gameManager.gameState != GameState.PLAY)
//            {
//                hoverHudCheck = false;
//            }

//            if (health > 0)
//            {
//                HoverHudUi();
//            }
//        }
//    }

//    private void NameCheck()
//    {
//        TextMeshProUGUI charaName = characterName.GetComponent<TextMeshProUGUI>();
//        charaName.text = gameObject.name;

//        if (gameObject.CompareTag("Enemy") && charaName.text.Contains("Guard"))
//        {
//            charaName.text = "Jade Guard";
//        }
//    }

//    private void HoverHudUi()
//    {
//        if (hoverHudCheck)
//        {
//            characterName.SetActive(true);
//            uniqueTurnHud.GetComponent<Animator>().SetBool("hover", true);

//            if (gameObject.CompareTag("Enemy") && !gameObject.GetComponent<_EnemyAction>().enemyStartTurn)
//            {
//                healthPanel.GetComponent<Animator>().SetBool("hover", true);
//            }
//        }
//        else
//        {
//            characterName.SetActive(false);
//            uniqueTurnHud.GetComponent<Animator>().SetBool("hover", false);

//            if (gameObject.CompareTag("Enemy") && !gameObject.GetComponent<_EnemyAction>().enemyStartTurn)
//            {
//                healthPanel.GetComponent<Animator>().SetBool("hover", false);
//            }
//        }
//    }

//    private void Death()
//    {
//        if (gameObject.CompareTag("Player"))
//        {
//            _PlayerAction player = GetComponent<_PlayerAction>();
//            statusEffectPanel.SetActive(false);
//            player.playerState = PlayerState.DYING;
//        }
//        else if (gameObject.CompareTag("Enemy"))
//        {
//            _EnemyAction enemy = GetComponent<_EnemyAction>();
//            statusEffectPanel.SetActive(false);
//            characterHpHud.GetComponent<Animator>().SetTrigger("death");
//            healthPanel.GetComponent<Animator>().SetTrigger("death");
//            enemy.enemyState = EnemyState.DYING;
//        }
//    }

//    public void TakeDamage(float damage, bool critCheck, string debuff)
//    {
//        health -= damage;

//        DamageText(damage, critCheck);
//        StatusText(debuff);
//        statusEffectHud.DisplayVfx(this.gameObject, debuff);

//        //set damage text
//        if (health <= 0)
//        {
//            health = 0;
//            if (hpValueUi != null) hpValueUi.text = health.ToString();
//            if (hpPercentUi != null) hpPercentUi.text = Mathf.RoundToInt(health / maxHealth * 100).ToString() + "%";

//            Death();
//        }
//        else
//        {
//            if (animator != null)
//            {
//                animator.SetTrigger("damaged");
//            }
//            if (hpValueUi != null) hpValueUi.text = health.ToString();
//            if (hpPercentUi != null) hpPercentUi.text = Mathf.RoundToInt(health / maxHealth * 100).ToString() + "%";
//        }

//        healthBarFill.fillAmount = health / maxHealth;
//    }

//    public void HealBuff(float buffAmount, bool cleanse)
//    {
//        health += buffAmount;

//        if (cleanse)
//        {
//            BuffText(buffAmount, "cleanse");
//            StatusText("cleanse");
//        }
//        else
//        {
//            BuffText(buffAmount, "heal");
//        }
//        if (health >= maxHealth)
//        {
//            health = maxHealth;
//        }

//        if (hpValueUi != null) hpValueUi.text = health.ToString();
//        if (hpPercentUi != null) hpPercentUi.text = Mathf.RoundToInt(health / maxHealth * 100).ToString() + "%";
//        healthBarFill.fillAmount = health / maxHealth;
//    }

//    #region Visual Effects
//    public void DamageText(float value, bool critCheck)
//    {
//        TextMeshProUGUI critText = floatingText.transform.Find("Crit Text").GetComponent<TextMeshProUGUI>();
//        TextMeshProUGUI damageText = floatingText.transform.Find("Text").GetComponent<TextMeshProUGUI>();

//        critText.color = Color.red;
//        damageText.color = Color.white;

//        if (critCheck)
//        {
//            critText.gameObject.SetActive(true);
//        }
//        else
//        {
//            critText.gameObject.SetActive(false);
//        }

//        damageText.text = value.ToString();
//        Instantiate(floatingText, transform.position, Quaternion.identity, transform);

//        damageText.text = null;
//    }

//    public void BuffText(float value, string effect)
//    {
//        TextMeshProUGUI critText = floatingText.transform.Find("Crit Text").GetComponent<TextMeshProUGUI>();
//        TextMeshProUGUI healText = floatingText.transform.Find("Text").GetComponent<TextMeshProUGUI>();
//        critText.gameObject.SetActive(false);
//        healText.color = Color.green;

//        switch (effect)
//        {
//            case "cleanse":
//                healText.text = "+" + value.ToString();
//                break;
//            case "heal":
//                healText.text = "+" + value.ToString();
//                break;
//            default:
//                healText.text = null;
//                break;
//        }

//        Instantiate(floatingText, transform.position, Quaternion.identity, transform);

//        healText.text = null;
//    }

//    public void StatusText(string effect)
//    {
//        TextMeshProUGUI popup = statusEffectText.GetComponentInChildren<TextMeshProUGUI>();
//        popup.color = Color.white;
//        Image popupImage = statusEffectText.transform.Find("Image").GetComponent<Image>();
//        popupImage.sprite = null;

//        switch (effect)
//        {
//            case "bleed":
//                popup.text = "Bleed";
//                popupImage.sprite = statusEffectHud.bleedIcon.transform.Find("Icon").GetComponent<Image>().sprite;
//                break;
//            case "shatter":
//                popup.text = "Shatter";
//                popupImage.sprite = statusEffectHud.shatterIcon.transform.Find("Icon").GetComponent<Image>().sprite;
//                break;
//            case "stun":
//                popup.text = "Stun";
//                popupImage.sprite = statusEffectHud.stunIcon.transform.Find("Icon").GetComponent<Image>().sprite;
//                break;
//            case "taunt":
//                popup.text = "Taunt";
//                popupImage.sprite = statusEffectHud.tauntIcon.transform.Find("Icon").GetComponent<Image>().sprite;
//                break;
//            case "cleanse":
//                popup.text = "Cleanse";
//                popupImage.sprite = statusEffectHud.cleanseIcon.transform.Find("Icon").GetComponent<Image>().sprite;
//                break;
//            case "enrage":
//                popup.text = "Enrage";
//                popupImage.sprite = statusEffectHud.enrageIcon.transform.Find("Icon").GetComponent<Image>().sprite;
//                break;
//            case "armor":
//                popup.text = "Armor";
//                popupImage.sprite = statusEffectHud.armorIcon.transform.Find("Icon").GetComponent<Image>().sprite;
//                break;
//            case "rage":
//                popup.text = "Berserk";
//                popupImage.sprite = statusEffectHud.rageGoatBuffIcon.transform.Find("Icon").GetComponent<Image>().sprite;
//                break;
//            default:
//                popup.text = null;
//                break;
//        }

//        if (effect != null)
//        {
//            Instantiate(statusEffectText, transform.position, Quaternion.identity, transform);
//        }

//        popup.text = null;
//    }
//    #endregion

//    #region Status Effects
//    public IEnumerator CheckStatusEffects()
//    {
//        //debuffs
//        #region Bleed
//        if (bleedStack > 0)
//        {
//            Bleed bleed = FindObjectOfType<Bleed>();
//            bleed.BleedCalculation(this);

//            TakeDamage(bleed.bleedDamage, false, "bleed");
//            --bleedStack;

//            statusEffectHud.UpdateEffectsBar(this, "bleed");
//        }
//        #endregion

//        #region Shatter
//        if (shatterCounter > 0)
//        {
//            --shatterCounter;

//            statusEffectHud.UpdateEffectsBar(this, "shatter");

//            if (shatterCounter <= 0)
//            {
//                defense += decreasedDefenseValue; //add back decreased value
//                decreasedDefenseValue = 0;
//            }
//        }
//        #endregion

//        #region Stun
//        if (stunCounter > 0)
//        {
//            StatusText("stun");
//        }
//        #endregion

//        #region Taunt
//        if (tauntCounter > 0)
//        {
//            StatusText("taunt");
//        }
//        #endregion

//        //buffs
//        #region Armor
//        if (armorCounter > 0)
//        {
//            --armorCounter;

//            statusEffectHud.UpdateEffectsBar(this, "armor");

//            if (armorCounter <= 0)
//            {
//                defense -= increasedDefenseValue; //minus back increased value
//                increasedDefenseValue = 0;
//            }
//        }
//        #endregion

//        yield return new WaitForSeconds(1f);

//        checkedStatus = true;
//    }

//    public void CheckEndStatusEffects()
//    {
//        //debuffs
//        #region Stun
//        if (stunCounter > 0)
//        {
//            --stunCounter;

//            statusEffectHud.UpdateEffectsBar(this, "stun");
//        }
//        #endregion

//        #region Taunt
//        if (tauntCounter > 0)
//        {
//            --tauntCounter;

//            statusEffectHud.UpdateEffectsBar(this, "taunt");
//        }
//        #endregion

//        //buffs
//        #region Speed Buff
//        if (speedCheck)
//        {
//            battleManager.revertingTurn = true;
//            speedCheck = false;
//        }
//        #endregion

//        #region Enrage
//        if (enrageCounter > 0)
//        {
//            --enrageCounter;

//            statusEffectHud.UpdateEffectsBar(this, "enrage");

//            if (enrageCounter <= 0)
//            {
//                attack -= increasedAttackValue;
//                increasedAttackValue = 0;
//            }
//        }
//        #endregion
//    }
//    #endregion

//    #region Mouse Detection
//    private void OnMouseEnter()
//    {
//        if (gameManager.gameState == GameState.PLAY && battleManager.battleState != BattleState.NEWGAME)
//        {
//            hoverHudCheck = true;
//        }
//    }

//    private void OnMouseExit()
//    {
//        if (gameManager.gameState == GameState.PLAY && battleManager.battleState != BattleState.NEWGAME)
//        {
//            hoverHudCheck = false;
//        }
//    }
//    #endregion
//}
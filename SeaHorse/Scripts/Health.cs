using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Health : MonoBehaviour
{
    public Text healthText;
    public Text playerText;
    public Text collectText;
    public int healthScore = 100;
    public int playerScore;
    public int collectScore = 0;
    public GameObject deathScreen;
    public GameObject player;
    public GameObject Pearl;
    public GameObject restart;

    private CrabSpawner spawner;

    public bool crabsLeft = true;
    // Start is called before the first frame update
    void Start()
    {
        collectScore = PlayerPrefs.GetInt("pearls");
        healthScore = PlayerPrefs.GetInt("health");
        playerScore = Random.Range(0, 20);
        spawner = FindObjectOfType<CrabSpawner>();
        healthText.text = "Health : " + healthScore + "hp";
        playerText.text = "Crabs Remaining : " + playerScore;
        collectText.text = "Pearls Collected : " + collectScore;
       
    }

    void Update()
    {
        if(healthScore <= 0)
        {
            player.SetActive(false);
            deathScreen.SetActive(true);
            restart.SetActive(true);
            Time.timeScale = 0.0f;
        }

        if (playerScore - spawner.currentCrabs == 0) crabsLeft = false;
    }
}
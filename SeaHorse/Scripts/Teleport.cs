using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class Teleport : MonoBehaviour
{
    public GameObject error;
    private Health health;
    private PlayerMovement move;
    public int scene;


    void Start()
    {
        health = FindObjectOfType<Health>();
        move = FindObjectOfType<PlayerMovement>();
    }

    void OnTriggerEnter2D(Collider2D other)
    {
        if (other.gameObject.tag == "Player" && health.playerScore != 0)
        {
            error.SetActive(true);
        }
        else
        {
            if (other.gameObject.tag != "Crab")
                 LoadNextLevel();
        }
    }

    void OnTriggerExit2D(Collider2D other)
    {
        if (other.gameObject.tag == "Player")
        {
            error.SetActive(false);
        }
    }

    void LoadNextLevel()
    {
        //Level to Level
        if (scene == 1) PlayerPrefs.SetInt("position", scene);
        if (scene == 2) PlayerPrefs.SetInt("position", scene);
        if (scene == 3) PlayerPrefs.SetInt("position", scene);
        if (scene == 4) PlayerPrefs.SetInt("position", scene);

        PlayerPrefs.SetInt("health", health.healthScore);
        PlayerPrefs.SetInt("pearls", health.collectScore);
        PlayerPrefs.SetInt("unicornSwitch", move.unicornSwitch);
        SceneManager.LoadScene("SampleScene");
    }
}
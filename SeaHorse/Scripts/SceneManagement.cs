using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class SceneManagement : MonoBehaviour
{
    
    public void LoadScene(int scene)
    {
        //Main Menu to Game
        if(scene == 1)
        {
            PlayerPrefs.SetInt("health", 3);
            PlayerPrefs.SetInt("unicornSwitch", 1);
            PlayerPrefs.SetInt("pearls", 0);
            PlayerPrefs.SetInt("position", 0);
            Time.timeScale = 1.0f;
            SceneManager.LoadScene("SampleScene");
        }

        //Quits the game
        if(scene == 2)
        {
            Application.Quit();
        }

        if (scene == 3)
        {
            SceneManager.LoadScene("MainMenu");
        }
    }
}
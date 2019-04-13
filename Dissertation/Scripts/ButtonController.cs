using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class ButtonController : MonoBehaviour
{
    public GameObject menu1;
    public GameObject menu2;
    public GameObject back;
    public GameObject instruct;

    public void LoadScene(int scene)
    {
        switch (scene){
            case 1:
                menu1.SetActive(false);
                menu2.SetActive(true);
                break;
            case 2:
                menu1.SetActive(true);
                menu2.SetActive(false);
                break;
            case 3:
                PlayerPrefs.SetInt("auto", 1);
                SceneManager.LoadScene("SampleScene");
                break;
            case 4:
                PlayerPrefs.SetInt("auto", 2);
                SceneManager.LoadScene("SampleScene");
                break;
            case 5:
                Application.Quit();
                break;
            case 6:
                PlayerPrefs.SetInt("auto", 1);
                PlayerPrefs.SetInt("speed", 2);
                SceneManager.LoadScene("turning");
                break;
            case 7:
                PlayerPrefs.SetInt("auto", 2);
                PlayerPrefs.SetInt("speed", 1);
                SceneManager.LoadScene("turning");
                break;
            case 8:
                SceneManager.LoadScene("menu");
                break;
            case 9:
                SceneManager.LoadScene("turning");
                break;
            case 10:
                menu1.SetActive(false);
                back.SetActive(true);
                instruct.SetActive(true);
                break;
            case 11:
                menu1.SetActive(true);
                back.SetActive(false);
                instruct.SetActive(false);
                break;
        };
    }
}
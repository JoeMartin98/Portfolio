using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

namespace UnityStandardAssets.Vehicles.Car
{
    public class carPositionController : MonoBehaviour
    {
        public GameObject leftSensor;
        public GameObject rightSensor;
        public GameObject frontSensor;
        public Text leftAverageText;
        public Text rightAverageText;
        public float temp1;
        public float temp2;
        public int temp;

        private float rightAdd;
        private float leftAdd;
        private float rightAverage;
        private float leftAverage;
        private int count;
        private courseController carInfo;
        private CarController carRealInfo;
        private CarUserControl carSteering;
        private Ray leftRay;
        private Ray rightRay;
        private Ray frontRay;
        private bool movement1 = false;
        private bool movement2 = false;
        private bool movement3 = false;  

        void Start()
        {
            count = 0;
            leftAdd = 0.0f;
            rightAdd = 0.0f;
            carInfo = FindObjectOfType<courseController>();
            carRealInfo = FindObjectOfType<CarController>();
            carSteering = FindObjectOfType<CarUserControl>();
            temp = PlayerPrefs.GetInt("auto");
        }

        void FixedUpdate()
        {
            if (carInfo.finished == false)
            {
                count++;
                RaycastHit hit;
                RaycastHit hitRight;
                RaycastHit hitFront;
                leftRay = new Ray(leftSensor.transform.position, transform.TransformDirection(Vector3.left));
                rightRay = new Ray(rightSensor.transform.position, transform.TransformDirection(Vector3.right));
                frontRay = new Ray(frontSensor.transform.position, transform.TransformDirection(Vector3.forward));

                if (Physics.Raycast(leftRay, out hit))
                {
                    leftAdd += hit.distance;
                    temp1 = hit.distance;
                }

                if (Physics.Raycast(rightRay, out hitRight))
                {
                    rightAdd += hitRight.distance;
                    temp2 = hitRight.distance;
                }

                if(temp == 1)
                {
                    if (hit.distance < 3.0f)
                    {
                        pushRight();
                        movement1 = true;
                    }
                    else movement1 = false;


                    if (hitRight.distance < 3.0f)
                    {
                        pushLeft();
                        movement2 = true;
                    }
                    else movement2 = false;

                    //FRONTTTT
                    if (Physics.Raycast(frontRay, out hitFront))
                    {
                        if (hitFront.distance < 13.0f)
                        {
                            movement3 = true;
                            if (temp1 > temp2) pushLeft();
                            else if (temp1 < temp2) pushRight();
                        }
                        else movement3 = false;
                    }

                    if (!movement3 && !movement2 && !movement1) carSteering.control = false;
                }             
            }
            else
            {
                if (!carRealInfo.crashed)
                {
                    rightAverage = rightAdd / count;
                    leftAverage = leftAdd / count;
                    rightAverageText.text = "Average distance to the right pavement = " + rightAverage.ToString("#.00") + "m";
                    leftAverageText.text = "Average distance to the left pavement = " + leftAverage.ToString("#.00") + "m";
                }
            }
        }

        void pushLeft()
        {
            carSteering.control = true;
            carRealInfo.Move(-.3f, 0, -0.08f, 0);
        }

        void pushRight()
        {
            carSteering.control = true;
            carRealInfo.Move(.3f, 0, -0.08f, 0);
        }
    }
}
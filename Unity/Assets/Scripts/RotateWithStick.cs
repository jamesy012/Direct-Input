using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RotateWithStick : MonoBehaviour {

    //speed multiplier for rotation
    public float m_Speed = 5;

    //Which controller should we look at
    public int m_ControllerIndex = 0;
    //Which controller Axes are we looking at
    public JInput.ControllerVec2Axes m_Axis;

    //bool to check if we should log axis info to the console
    public bool m_PrintInfoToConsole = false;

	// Update is called once per frame
	void Update () {
        //get the controller from the index
        JInput.Controller controller = JInput.InputManager.getController(m_ControllerIndex);

        //controller can be null
        if(controller == null) {
            return;
        }

        //get axis
        Vector2 axisVec;
        //axisVec.x = controller.getAxisValue(JInput.ControllerAxes.LStickX);
        //axisVec.y = controller.getAxisValue(JInput.ControllerAxes.LStickY);
        axisVec = controller.getAxisValue(m_Axis);

        //axis values are backwards from what we want
        axisVec = -axisVec;

        //rotate the transform
        transform.Rotate(new Vector3(axisVec.y,axisVec.x,0) * m_Speed, Space.World);

        if (m_PrintInfoToConsole) {
            print(m_ControllerIndex + " - " + m_Axis.ToString() + " - (" + axisVec.x + "," + axisVec.y + ")");
        }

    }
}

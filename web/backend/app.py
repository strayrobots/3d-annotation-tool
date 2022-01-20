from flask import Flask, make_response, send_file, jsonify, request
from flask.views import MethodView
import os
from flask_cors import CORS
import json
from straylib.scene import Scene
import cv2
import numpy as np

app = Flask(__name__, static_url_path='/assets', static_folder='assets')
CORS(app)
root = os.getenv('SCENE_DIR', "./")


def find_point_3d(scene, idx_1, idx_2, kp_1, kp_2):
    camera = scene.camera().scale((640,480))

    T_AW = np.linalg.inv(scene.poses[idx_1])
    T_BW = np.linalg.inv(scene.poses[idx_2])

    PA = camera.camera_matrix  @ T_AW[:3]
    PB = camera.camera_matrix  @ T_BW[:3]

    point_3d = cv2.triangulatePoints(PA, PB, kp_1, kp_2)

    point_3d /= point_3d[3, :]

    return point_3d

class PathView(MethodView):
    def get(self, p=''):
        path = os.path.join(root, "color", p)
        if p == '':
            frames = []
            for file in sorted(os.listdir(path)):
                idx = int(file.split(".")[0])
                keypoints = []
                frame = dict(file=file, idx=idx, keypoints=keypoints)
                frames.append(frame)

            return jsonify(frames)
        if os.path.isfile(path):
            return send_file(path)
        else:
            res = make_response('File not found', 404)
        return res

    def post(self, p=''):
        if p == 'triangulate':
            payload = request.get_json()
            frame_1 = payload.get("frame_1")
            frame_2 = payload.get("frame_2")
            scene = Scene(root)
            points_3d = dict()
            for kp_1 in frame_1["keypoints"]:
                for kp_2 in frame_2["keypoints"]:
                    if kp_1["id"] == kp_2["id"]:
                        kp_1_np = np.array([kp_1["x"], kp_1["y"]], dtype=np.float32)
                        kp_2_np = np.array([kp_2["x"], kp_2["y"]], dtype=np.float32)
                        points_3d[kp_1["id"]] = dict(category_id=kp_1["categoryId"], point=find_point_3d(scene, frame_1["idx"], frame_2["idx"], kp_1_np, kp_2_np).tolist())

            projected = []

            camera = scene.camera().scale((640,480))
            files = [os.path.basename(path) for path in scene.get_image_filepaths()]

            for i, pose in enumerate(scene.poses):
                file = files[i]
                idx = int(file.split(".")[0])
                frame = dict(file=file, idx=idx, keypoints=[])
                for id, kp_3d in points_3d.items():
                    kp = dict(is_dragging=False, id=id, categoryId=kp_3d["category_id"])
                    kp_2d = camera.project(np.array(kp_3d["point"][:3]), np.linalg.inv(pose))
                    kp["x"] = int(kp_2d[0][0])
                    kp["y"] = int(kp_2d[0][1])
                    frame["keypoints"].append(kp)

                projected.append(frame)
            
            res = make_response(json.JSONEncoder().encode(projected), 200)
            res.headers.add('Content-type', 'application/json')

        else:
            res = make_response('Not found', 404)
        return res


path_view = PathView.as_view('path_view')
app.add_url_rule('/', view_func=path_view)
app.add_url_rule('/<path:p>', view_func=path_view)


if __name__ == '__main__':
    bind = os.getenv('FS_BIND', '0.0.0.0')
    port = os.getenv('FS_PORT', '8000')
    app.run(bind, port, threaded=True, debug=False)

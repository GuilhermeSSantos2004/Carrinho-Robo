from pathlib import Path
import json,struct
import numpy as np
ROOT=Path(__file__).resolve().parents[1]

def stl_tri(path):
    data=Path(path).read_bytes(); n=struct.unpack('<I',data[80:84])[0]
    assert len(data)==84+n*50
    dt=np.dtype([('n','<f4',3),('v','<f4',(3,3)),('attr','<u2')])
    return np.frombuffer(data,dtype=dt,offset=84,count=n)['v'].astype(np.float64)

def verify_mesh(path):
    tri=stl_tri(path)
    verts,idx=np.unique(np.round(tri.reshape(-1,3),5),axis=0,return_inverse=True)
    idx=idx.reshape(-1,3)
    edges=np.concatenate((idx[:,[0,1]],idx[:,[1,2]],idx[:,[2,0]]))
    _,ct=np.unique(np.sort(edges,axis=1),axis=0,return_counts=True)
    area=np.linalg.norm(np.cross(tri[:,1]-tri[:,0],tri[:,2]-tri[:,0]),axis=1)/2
    vol=np.sum(np.einsum('ij,ij->i',tri[:,0],np.cross(tri[:,1],tri[:,2])))/6
    return {'triangles':len(tri),'vertices':len(verts),'open_edges':int(np.sum(ct==1)),
            'nonmanifold_edges':int(np.sum(ct>2)),'degenerate_triangles':int(np.sum(area<1e-8)),
            'signed_volume_mm3':round(float(vol),3),'bounds_mm':[tri.reshape(-1,3).min(0).round(3).tolist(),tri.reshape(-1,3).max(0).round(3).tolist()],
            'watertight':bool(np.all(ct==2)),'positive_volume':bool(vol>0)}


def main():
    results=[]
    for path in sorted(ROOT.rglob('*.stl')):
        result=verify_mesh(path);result['file']=str(path.relative_to(ROOT));results.append(result)
        assert result['watertight'] and result['positive_volume'] and result['degenerate_triangles']==0,result
    (ROOT/'VALIDACAO/validacao_malhas.json').write_text(json.dumps(results,indent=2))
    print('Malhas aprovadas:',len(results))
if __name__=='__main__': main()

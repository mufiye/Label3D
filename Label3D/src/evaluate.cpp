#include "evaluate.h"

//����GroundTruth
void Evaluation::LoadGroundTruthData(const string& gtFile)
{
	groundTruth = filetool::ReadTxtFileInt(gtFile);
	insFaces_gt = GetInsFIndices(groundTruth);
}

//����Ԥ����Prediction
void Evaluation::LoadPredictionData(const string& predFile)
{
	prediction = filetool::ReadTxtFileInt(predFile);
	insFaces_pred = GetInsFIndices(prediction);
}

void Evaluation::LoadData(const string& gtFile, const string& predFile)
{
	groundTruth = filetool::ReadTxtFileInt(gtFile);
	prediction = filetool::ReadTxtFileInt(predFile);

	insFaces_gt = GetInsFIndices(groundTruth);
	insFaces_pred = GetInsFIndices(prediction);
}

//��ȡÿ��ʵ����������Ƭ����
vector<vector<int>> Evaluation::GetInsFIndices(const vector<int>& label)
{
	int maxIndex = *std::max_element(label.begin(), label.end());
	int insCount = maxIndex + 1;
	vector<vector<int>> insFaces(insCount);
	int numFaces = label.size();
	for (size_t i = 0; i < numFaces; i++)
	{
		if (label[i] >= 0)
		{
			insFaces[label[i]].push_back(i);
		}
	}

	vector<vector<int>> insFaces_NoZero;
	for (size_t i = 0; i < insCount; i++)
	{
		if (insFaces[i].size() > 0)
		{
			insFaces_NoZero.push_back(insFaces[i]);
		}
	}

	return insFaces_NoZero;
}

//ȡ�������������Ľ���
vector<int> Evaluation::GetIntersection(const vector<int>& a, const vector<int>& b)
{
	vector<int> c;
	vector<int>::iterator it;
	set_intersection(a.begin(), a.end(), b.begin(), b.end(), back_inserter(c));
	return c;
}

//ȡ�������������Ĳ���
vector<int> Evaluation::GetUnion(const vector<int>& a, const vector<int>& b)
{
	vector<int> c;
	vector<int>::iterator it;
	set_union(a.begin(), a.end(), b.begin(), b.end(), back_inserter(c));
	return c;
}

//����ÿ��Ԥ���ʵ������ʵʵ��֮���iou,����siou[pred_count][gt_count]
vector<vector<Evaluation::SIou>> Evaluation::CalcIou_Pred()
{
	int insCount_gt = insFaces_gt.size();
	int insCount_pred = insFaces_pred.size();

	vector<vector<SIou>> ious_pred(insCount_pred);	//��¼ÿ��Ԥ��ʵ����iou
	for (size_t pi = 0; pi < insCount_pred; pi++)
	{
		vector<SIou> ious_pi(insCount_gt);
		for (size_t gi = 0; gi < insCount_gt; gi++)
		{
			vector<int> &faces_Pred = insFaces_pred[pi];
			vector<int> &faces_Gt = insFaces_gt[gi];

			vector<int> f_inter = GetIntersection(faces_Pred, faces_Gt);	//����
			vector<int> f_union = GetUnion(faces_Pred, faces_Gt);	//����

			double area_inter = 0, area_union = 0;
			size_t size_inter = f_inter.size();
			size_t size_union = f_union.size();
			for (size_t fi = 0; fi < size_inter; fi++)
			{
				area_inter += faceArea[f_inter[fi]];
			}
			for (size_t fi = 0; fi < size_union; fi++)
			{
				area_union += faceArea[f_union[fi]];
			}

			double iou = area_union > 0 ? area_inter / area_union : 0;
			ious_pi[gi].pred_id = pi;
			ious_pi[gi].gt_id = gi;
			ious_pi[gi].iou = iou;
		}
		ious_pred[pi] = ious_pi;
	}

	return ious_pred;
}

//����ÿ����ʵʵ����Ԥ���ʵ��֮���iou,����siou[gt_count][pred_count]
vector<vector<Evaluation::SIou>> Evaluation::CalcIou_GT()
{
	int insCount_gt = insFaces_gt.size();
	int insCount_pred = insFaces_pred.size();

	vector<vector<SIou>> ious_gt(insCount_gt);	//��¼ÿ��Ԥ��ʵ����iou
	for (size_t gi = 0; gi < insCount_gt; gi++)
	{
		vector<SIou> ious_gi(insCount_pred);
		for (size_t pi = 0; pi < insCount_pred; pi++)
		{
			vector<int> &faces_Pred = insFaces_pred[pi];
			vector<int> &faces_Gt = insFaces_gt[gi];

			vector<int> f_inter = GetIntersection(faces_Pred, faces_Gt);	//����
			vector<int> f_union = GetUnion(faces_Pred, faces_Gt);	//����

			double area_inter = 0, area_union = 0;
			size_t size_inter = f_inter.size();
			size_t size_union = f_union.size();
			for (size_t fi = 0; fi < area_inter; fi++)
			{
				area_inter += faceArea[f_inter[fi]];
			}
			for (size_t fi = 0; fi < size_union; fi++)
			{
				area_union += faceArea[f_union[fi]];
			}

			double iou = area_union > 0 ? area_inter / area_union : 0;
			ious_gi[gi].pred_id = pi;
			ious_gi[gi].gt_id = gi;
			ious_gi[gi].iou = iou;
		}
		ious_gt[gi] = ious_gi;
	}

	return ious_gt;
}

//����Ƚϣ�ȡmaxElement��
bool Evaluation::iouCmp_asce(Evaluation::SIou a, Evaluation::SIou b)
{
	if (a.iou < b.iou )
		return true;
	return false;
}

//����Ƚϣ�ȡminElement��
bool Evaluation::iouCmp_desc(Evaluation::SIou a, Evaluation::SIou b)
{
	if (a.iou > b.iou)
		return true;
	return false;
}

//����׼ȷ��Average Precision
void Evaluation::CalcAverPrecision(float minIou)
{
	int insCount_gt = insFaces_gt.size();
	int insCount_pred = insFaces_pred.size();
	
	//��һ��������ÿ��Ԥ���ʵ����GTʵ��֮���iou��
	vector<vector<SIou>> iousTable = CalcIou_Pred();
	for (size_t i = 0; i < insCount_pred; i++)
	{
		sort(iousTable[i].begin(), iousTable[i].end(), iouCmp_desc); //��������
	}


	/*
	�ڶ�������iousTable��ɸѡ����ÿ��Ԥ���ʵ����Ӧ��GroundTruthʵ����
	ע��:ÿ��GroundTruthʵ��ֻ�ܶ�Ӧ��һ��Ԥ���ʵ��
	*/

	vector<SIou> iousList_pred(insCount_pred);	//��¼��ÿ��predԤ��ʵ����Ե���ʵʵ�������ս��
	for (size_t i = 0; i < insCount_pred; i++)
	{
		iousList_pred[i].pred_id = i;
		iousList_pred[i].gt_id = -1;
		iousList_pred[i].iou = 0;
	}

	vector<SIou> iousList_gt(insCount_gt);	//��¼��ÿ����ʵʵ����Ե�predԤ��ʵ��,�����������ʱ����
	for (size_t i = 0; i < insCount_gt; i++)
	{
		iousList_gt[i].pred_id = -1;
		iousList_gt[i].gt_id = i;
		iousList_gt[i].iou = 0;
	}

	vector<bool> ifNeedMatch(insCount_pred);	//��¼ÿ��Ԥ���ʵ������ʵʵ��������Ƿ���Ҫ����
	for (size_t i = 0; i < insCount_pred; i++)
	{
		ifNeedMatch[i] = true;
	}
	bool ifUpdated =true;	//�Ƿ��и���
	do
	{
		ifUpdated = false;
		for (size_t pi = 0; pi < insCount_pred; pi++)
		{
			if (!ifNeedMatch[pi])
			{
				continue;
			}
			vector<SIou> &iousWithGt = iousTable[pi];
			//��iousWithGt��ѡȡ���ֵ��Ϊ��ʵ�����յ�iou
			//std::vector<SIou>::iterator biggest = std::max_element(std::begin(iousWithGt), std::end(iousWithGt), iouCmp);

			for (size_t gi = 0; gi < insCount_gt; gi++)
			{
				SIou maxIou = iousWithGt[gi];
				if (maxIou.iou >= minIou)
				{
					/*
					�ж�maxIou.gt_id��ָ�����ʵʵ���Ƿ��Ѿ���predԤ��ʵ����ԣ�
					�����򽫸�gt_id�뵱ǰpiָ���predԤ��ʵ�����
					���ǣ���Ƚϵ�ǰmaxIou.iou�롰����ʵʵ������Ե�predԤ��ʵ��ֱ�ӵ�iou��
					*/
					if (iousList_gt[maxIou.gt_id].pred_id < 0)	//<0,��-1˵����maxIou.gt_idʵ����δ���
					{
						iousList_gt[maxIou.gt_id].pred_id = pi;
						iousList_gt[maxIou.gt_id].iou = maxIou.iou;

						iousList_pred[pi] = maxIou;
						ifUpdated = true;
						break;
					}
					else
					{
						if (iousList_gt[maxIou.gt_id].iou < maxIou.iou)	//�����ǰmaxIou.iou���������ʵ����iou�������
						{
							ifNeedMatch[iousList_gt[maxIou.gt_id].pred_id] = true;
							iousList_gt[maxIou.gt_id].pred_id = pi;
							iousList_gt[maxIou.gt_id].iou = maxIou.iou;

							iousList_pred[iousList_gt[maxIou.gt_id].pred_id].gt_id = -1;
							iousList_pred[iousList_gt[maxIou.gt_id].pred_id].iou = 0;
							iousList_pred[pi] = maxIou;
							ifUpdated = true;
							break;
						}
						else //����
						{
							continue;
						}
					}
				}
				else
				{
					ifNeedMatch[pi] = false;
					break;
				}
			}
			ifNeedMatch[pi] = false;
		}
	} while (ifUpdated);


	//������������ֵminIou����True Positives, False Positives, True Negatives, False Negatives
	vector<bool> pos_neg_pred(insCount_pred);	//��¼Ԥ��ʵ�������������Ǹ�����
	vector<bool> pos_neg_gt(insCount_gt);	//��¼Ԥ��ʵ�������������Ǹ�����
	for (size_t i = 0; i < insCount_pred; i++)
	{
		pos_neg_pred[i] = false;
	}
	for (size_t i = 0; i < insCount_gt; i++)
	{
		pos_neg_gt[i] = false;
	}

	for (size_t pi = 0; pi < insCount_pred; pi++)
	{
		if (iousList_pred[pi].gt_id > 0)
		{
			pos_neg_pred[pi] = true;
			pos_neg_gt[iousList_pred[pi].gt_id] = true;
		}
	}

	int truePositives = 0, falsePositives = 0, trueNegatives = 0, falseNegatives = 0;
	for (size_t pi = 0; pi < insCount_pred; pi++)
	{
		if (pos_neg_pred[pi])
		{
			truePositives++;
		}
		else
		{
			falsePositives++;
		}
	}

	for (size_t gi = 0; gi < insCount_gt; gi++)
	{
		if (pos_neg_gt[gi])
		{
			trueNegatives++;
		}
		else
		{
			falseNegatives++;
		}
	}

	double precision = double(truePositives)/double(insCount_pred) ;	//��ȷ��(Precision)
	double recall = double(truePositives) / double(insCount_gt);	//�ٻ���(Recall)

	std::cout << "��ȷ��(Precision)@miou=" << minIou << ": " << precision << endl;;
	std::cout << "�ٻ���(Recall)@miou=" << minIou << ": " << recall << endl;
}

void Evaluation::TestGetMaxElement()
{
	vector<SIou> ious(4);
	ious[0].gt_id = 1;
	ious[0].iou = 3.0;
	ious[1].gt_id = 4;
	ious[1].iou = 8.0;
	ious[2].gt_id = 2;
	ious[2].iou = 15.0;
	ious[3].gt_id = 5;
	ious[3].iou = 2.0;

	std::vector<SIou>::iterator biggest = std::max_element(std::begin(ious), std::end(ious), iouCmp_asce);
	std::cout << "Max element is " << (*biggest).iou << " at position " << std::distance(std::begin(ious), biggest) << std::endl;
}

vector<CBoundingBox> CalcInsBoxs(const vector<vector<int>>& insFaces, const GLMmodel *model)
{
	vector<CBoundingBox> insBBoxs;

	int insCount = insFaces.size();
	if (insCount <= 0)
	{
		std::cout << "���ȼ��طָ�����" << endl << endl;
		return insBBoxs;
	}
	insBBoxs.resize(insCount);

	for (size_t i = 0; i < insCount; i++)
	{
		vector<Vector3d> points;
		int includeFSize = insFaces[i].size();
		if (includeFSize >0)
		{
			for (size_t fi = 0; fi < includeFSize; fi++)
			{
				int faceIndex = insFaces[i][fi];
				for (size_t k = 0; k < 3; k++)
				{
					double x = model->vertices[3 * model->triangles[faceIndex].vindices[k] + 0];
					double y = model->vertices[3 * model->triangles[faceIndex].vindices[k] + 1];
					double z = model->vertices[3 * model->triangles[faceIndex].vindices[k] + 2];
					points.push_back(Vector3d(x, y, z));
				}
			}
			insBBoxs[i].InitBox(points);
		}
	}

	return insBBoxs;
}

//��ȡ��δʶ���ʵ����Χ��
void Evaluation::GetErrorMissInsBox(vector<CBoundingBox> &errorBoxs, vector<CBoundingBox> &missBoxs, const GLMmodel *model, float minIou)
{
	errorBoxs.clear();
	missBoxs.clear();

	vector<CBoundingBox> errorBoxs_;
	vector<CBoundingBox> missBoxs_;

	int insCount_gt = insFaces_gt.size();
	int insCount_pred = insFaces_pred.size();

	vector<CBoundingBox> boxs_gt = CalcInsBoxs(insFaces_gt, model);
	vector<CBoundingBox> boxs_pred = CalcInsBoxs(insFaces_pred, model);

	//��һ��������ÿ��Ԥ���ʵ����GTʵ��֮���iou��
	vector<vector<SIou>> iousTable = CalcIou_Pred();
	for (size_t i = 0; i < insCount_pred; i++)
	{
		sort(iousTable[i].begin(), iousTable[i].end(), iouCmp_desc); //��������
	}


	/*
	�ڶ�������iousTable��ɸѡ����ÿ��Ԥ���ʵ����Ӧ��GroundTruthʵ����
	ע��:ÿ��GroundTruthʵ��ֻ�ܶ�Ӧ��һ��Ԥ���ʵ��
	*/

	vector<SIou> iousList_pred(insCount_pred);	//��¼��ÿ��predԤ��ʵ����Ե���ʵʵ�������ս��
	for (size_t i = 0; i < insCount_pred; i++)
	{
		iousList_pred[i].pred_id = i;
		iousList_pred[i].gt_id = -1;
		iousList_pred[i].iou = 0;
	}

	vector<SIou> iousList_gt(insCount_gt);	//��¼��ÿ����ʵʵ����Ե�predԤ��ʵ��,�����������ʱ����
	for (size_t i = 0; i < insCount_gt; i++)
	{
		iousList_gt[i].pred_id = -1;
		iousList_gt[i].gt_id = i;
		iousList_gt[i].iou = 0;
	}

	vector<bool> ifNeedMatch(insCount_pred);	//��¼ÿ��Ԥ���ʵ������ʵʵ��������Ƿ���Ҫ����
	for (size_t i = 0; i < insCount_pred; i++)
	{
		ifNeedMatch[i] = true;
	}
	bool ifUpdated = true;	//�Ƿ��и���
	do
	{
		ifUpdated = false;
		for (size_t pi = 0; pi < insCount_pred; pi++)
		{
			if (!ifNeedMatch[pi])
			{
				continue;
			}
			vector<SIou> &iousWithGt = iousTable[pi];
			//��iousWithGt��ѡȡ���ֵ��Ϊ��ʵ�����յ�iou
			//std::vector<SIou>::iterator biggest = std::max_element(std::begin(iousWithGt), std::end(iousWithGt), iouCmp);

			for (size_t gi = 0; gi < insCount_gt; gi++)
			{
				SIou maxIou = iousWithGt[gi];
				if (maxIou.iou >= minIou)
				{
					/*
					�ж�maxIou.gt_id��ָ�����ʵʵ���Ƿ��Ѿ���predԤ��ʵ����ԣ�
					�����򽫸�gt_id�뵱ǰpiָ���predԤ��ʵ�����
					���ǣ���Ƚϵ�ǰmaxIou.iou�롰����ʵʵ������Ե�predԤ��ʵ��ֱ�ӵ�iou��
					*/
					if (iousList_gt[maxIou.gt_id].pred_id < 0)	//<0,��-1˵����maxIou.gt_idʵ����δ���
					{
						iousList_gt[maxIou.gt_id].pred_id = pi;
						iousList_gt[maxIou.gt_id].iou = maxIou.iou;

						iousList_pred[pi] = maxIou;
						ifUpdated = true;
						break;
					}
					else
					{
						if (iousList_gt[maxIou.gt_id].iou < maxIou.iou)	//�����ǰmaxIou.iou���������ʵ����iou�������
						{
							ifNeedMatch[iousList_gt[maxIou.gt_id].pred_id] = true;
							iousList_gt[maxIou.gt_id].pred_id = pi;
							iousList_gt[maxIou.gt_id].iou = maxIou.iou;

							iousList_pred[iousList_gt[maxIou.gt_id].pred_id].gt_id = -1;
							iousList_pred[iousList_gt[maxIou.gt_id].pred_id].iou = 0;
							iousList_pred[pi] = maxIou;
							ifUpdated = true;
							break;
						}
						else //����
						{
							continue;
						}
					}
				}
				else
				{
					ifNeedMatch[pi] = false;
					break;
				}
			}
			ifNeedMatch[pi] = false;
		}
	} while (ifUpdated);


	//������������ֵminIou����True Positives, False Positives, True Negatives, False Negatives
	vector<bool> pos_neg_pred(insCount_pred);	//��¼Ԥ��ʵ�������������Ǹ�����
	vector<bool> pos_neg_gt(insCount_gt);	//��¼Ԥ��ʵ�������������Ǹ�����
	for (size_t i = 0; i < insCount_pred; i++)
	{
		pos_neg_pred[i] = false;
	}
	for (size_t i = 0; i < insCount_gt; i++)
	{
		pos_neg_gt[i] = false;
	}

	for (size_t pi = 0; pi < insCount_pred; pi++)
	{
		if (iousList_pred[pi].gt_id > 0)
		{
			pos_neg_pred[pi] = true;
			pos_neg_gt[iousList_pred[pi].gt_id] = true;
		}
	}

	int truePositives = 0, falsePositives = 0, trueNegatives = 0, falseNegatives = 0;
	for (size_t pi = 0; pi < insCount_pred; pi++)
	{
		if (pos_neg_pred[pi])
		{
			truePositives++;
		}
		else
		{
			falsePositives++;
			errorBoxs_.push_back(boxs_pred[pi]);
		}
	}

	for (size_t gi = 0; gi < insCount_gt; gi++)
	{
		if (pos_neg_gt[gi])
		{
			trueNegatives++;
		}
		else
		{
			falseNegatives++;
			missBoxs_.push_back(boxs_gt[gi]);
		}
	}

	double precision = double(truePositives) / double(insCount_pred);	//��ȷ��(Precision)
	double recall = double(truePositives) / double(insCount_gt);	//�ٻ���(Recall)

	std::cout << "��ȷ��(Precision)@miou=" << minIou << ": " << precision << endl;;
	std::cout << "�ٻ���(Recall)@miou=" << minIou << ": " << recall << endl;

	errorBoxs = errorBoxs_;
	missBoxs = missBoxs_;
}

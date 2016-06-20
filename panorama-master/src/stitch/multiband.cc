//File: multiband.cc
//Author: Yuxin Wu <ppwwyyxx@gmail.com>

#include "multiband.hh"
#include "lib/imgproc.hh"
#include "feature/gaussian.hh"

namespace pano {

void MultiBandBlender::add_image(
			const Coor& upper_left,
			const Coor& bottom_right,
			const Mat32f &img,
			std::function<Vec2D(Coor)> coor_func) {
	Range range{upper_left, bottom_right};
	Mat<WeightedPixel> wimg(range.height(), range.width(), 1);
	for (int i = 0; i < range.height(); ++i)
		for (int j = 0; j < range.width(); ++j) {
			Coor target_coor{j + range.min.x, i + range.min.y};
			Vec2D orig_coor = coor_func(target_coor);
			Color c = interpolate(img, orig_coor.y, orig_coor.x);
			wimg.at(i, j).c = c;
			if (c.x < 0) {	// Color::NO
				wimg.at(i, j).w = 0;
			} else {
				orig_coor.x = orig_coor.x / img.width() - 0.5;
				orig_coor.y = orig_coor.y / img.height() - 0.5;
				wimg.at(i, j).w = std::max(
						(0.5f - fabs(orig_coor.x)) * (0.5f - fabs(orig_coor.y)),
						0.0) + EPS;
				// ext? eps?
			}
		}
	images.emplace_back(ImageToBlend{range, move(wimg)});
	target_size.update_max(bottom_right);
}

Mat32f MultiBandBlender::run() {
	update_weight_map();
	Mat32f target(target_size.y, target_size.x, 3);
	fill(target, Color::NO);

	next_lvl_images.resize(images.size());
	for (int level = 0; level < band_level; level ++) {
		create_next_level(level);
		//debug_level(level);
		REP(i, target_size.y) REP(j, target_size.x) {
			Color isum(0, 0, 0);
			float wsum = 0;
			REP(imgid, images.size())  {
				auto& img_cur = images[imgid];
				if (img_cur.range.contain(i, j)) {
					auto& ccur = img_cur.color_on_target(j, i);
					if (ccur.get_min() < 0) continue;
					auto & img_next = next_lvl_images[imgid];
					float w = img_cur.weight_on_target(j, i);
					if (w <= 0) continue;
					isum += (ccur - img_next.color_on_target(j, i)) * w;
					wsum += w;
				}
			}
			if (wsum < EPS)
				continue;
			isum /= wsum;
			float* p = target.ptr(i, j);
			if (*p < 0) {
				isum.write_to(p);
			} else {
				p[0] += isum.x, p[1] += isum.y, p[2] += isum.z;
			}
		}
		swap(next_lvl_images, images);
	}

	// XXX
	REP(i, target.rows()) REP(j, target.cols()) {
		float* p = target.ptr(i, j);
		update_min(p[0], 1.0f);
		update_min(p[1], 1.0f);
		update_min(p[2], 1.0f);
	}
	return target;
}

void MultiBandBlender::update_weight_map() {
	REP(i, target_size.y) REP(j, target_size.x) {
		float max = 0;
		for (auto& img : images)
			if (img.range.contain(i, j))
				update_max(max, img.weight_on_target(j, i));
		max -= EPS;
		for (auto& img : images) if (img.range.contain(i, j)) {
			float& w = img.weight_on_target(j, i);
			w = (w >= max);
		}
	}
}

void MultiBandBlender::create_next_level(int level) {
	m_assert(next_lvl_images.size() == images.size());
	if (level == band_level - 1) {
		REP(k, images.size()) {
			auto& img = images[k].img;
			next_lvl_images[k].range = images[k].range;
			// don't have to do this when band_level > 1
			auto& wimg = (next_lvl_images[k].img = Mat<WeightedPixel>(img.rows(), img.cols(), 1));
			REP(i, img.rows()) REP(j, img.cols()) {
				wimg.at(i, j).c = Color::BLACK;
				wimg.at(i, j).w = 0;
			}
			//memset(wimg.ptr(), 0, sizeof(float) * 4 * img.rows() * img.cols());
		}
	} else {
		GaussianBlur blurer(sqrt(level * 2 + 1.0) * 3);
		REP(i, images.size()) {
			next_lvl_images[i].range = images[i].range;
			next_lvl_images[i].img = blurer.blur(images[i].img);
		}
	}
}


}	// namespace pano

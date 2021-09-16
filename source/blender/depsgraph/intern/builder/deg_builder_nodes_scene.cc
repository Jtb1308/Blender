/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2013 Blender Foundation.
 * All rights reserved.
 */

/** \file
 * \ingroup depsgraph
 */

#include "intern/builder/deg_builder_nodes.h"

#include "DNA_scene_types.h"

#include "BKE_scene.h"

namespace blender::deg {

void DepsgraphNodeBuilder::build_scene_render(Scene *scene, ViewLayer *view_layer)
{
  scene_ = scene;
  view_layer_ = view_layer;
  const bool build_compositor = (scene->r.scemode & R_DOCOMP);
  const bool build_sequencer = (scene->r.scemode & R_DOSEQ);
  IDNode *id_node = add_id_node(&scene->id);
  id_node->linked_state = DEG_ID_LINKED_DIRECTLY;
  add_time_source();
  build_animdata(&scene->id);
  build_scene_parameters(scene);
  build_scene_audio(scene);
  if (build_compositor) {
    build_scene_compositor(scene);
  }
  if (build_sequencer) {
    build_scene_sequencer(scene);
    build_scene_speakers(scene, view_layer);
  }
  if (scene->camera != nullptr) {
    build_object(-1, scene->camera, DEG_ID_LINKED_DIRECTLY, true);
  }
}

void DepsgraphNodeBuilder::build_scene_parameters(Scene *scene)
{
  if (built_map_.checkIsBuiltAndTag(scene, BuilderMap::TAG_PARAMETERS)) {
    return;
  }
  build_parameters(&scene->id);
  build_idproperties(scene->id.properties);
  add_operation_node(&scene->id, NodeType::PARAMETERS, OperationCode::SCENE_EVAL);
  /* NOTE: This is a bit overkill and can potentially pull a bit too much into the graph, but:
   *
   * - We definitely need an ID node for the scene's compositor, otherwise re-mapping will no
   *   happen correct and we will risk remapping pointers in the main database.
   * - Alternatively, we should discard compositor tree, but this might cause other headache like
   *   drivers which are coming from the tree.
   *
   * Would be nice to find some reliable way of ignoring compositor here, but it's already pulled
   * in when building scene from view layer, so this particular case does not make things
   * marginally worse. */
  build_scene_compositor(scene);

  LISTBASE_FOREACH (TimeMarker *, marker, &scene->markers) {
    build_idproperties(marker->prop);
  }
}

void DepsgraphNodeBuilder::build_scene_compositor(Scene *scene)
{
  if (built_map_.checkIsBuiltAndTag(scene, BuilderMap::TAG_SCENE_COMPOSITOR)) {
    return;
  }
  if (scene->nodetree == nullptr) {
    return;
  }
  Scene *scene_cow = get_cow_datablock(scene);
  add_operation_node(&scene->id,
                     NodeType::SHADING,
                     OperationCode::COMPOSITOR_EVAL,
                     [scene_cow](::Depsgraph *depsgraph) {
                       BKE_scene_eval_compositor_nodetree(depsgraph, scene_cow);
                     });

  build_nodetree(scene->nodetree);
}

}  // namespace blender::deg
